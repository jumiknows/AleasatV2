/**
 * @file quest.c
 * @brief Quest attitude determination algorithm
 */

/******************************************************************************/
/*                              I N C L U D E S                               */
/******************************************************************************/

//QUEST
#include "quest.h"

// Utils
#include "obc_utils.h"
#include "quaternion.h"

// DSPLIB
#include "dsp_math.h"

/******************************************************************************/
/*                               D E F I N E S                                */
/******************************************************************************/

#define NEWTON_ITER_TOLERANCE 10E-10
#define NEWTON_ITER_LIMIT 10U

/******************************************************************************/
/*            P R I V A T E  F U N C T I O N  P R O T O T Y P E S             */
/******************************************************************************/

static float32_t trace3x3(const arm_matrix_instance_f32 *mat);
static float32_t det3x3(const arm_matrix_instance_f32 *mat);
static void normalize(float32_t *vec, uint8_t n);
static float32_t dot3(const float32_t *v1, const float32_t *v2);

/******************************************************************************/
/*               P R I V A T E  G L O B A L  V A R I A B L E S                */
/******************************************************************************/
//mat data for all other temp array instances
static float32_t mat_data[21] = {0.0f};

/******************************************************************************/
/*                       P U B L I C  F U N C T I O N S                       */
/******************************************************************************/

/**
 * @brief Estimates atttidude quaternion of the cubesat with the quest algorithm (QUaternion ESTimator)
 * Requires inputs from sun sensor, magnetometer, sun ephemeris model, and magnetic models.
 * The output attitude quaternion represents the  rotation from body frame -> inertial frame.
 * 
 * References:
 *     - Shuster, Malcom D. Approximate Algorithms for Fast Optimal Attitude Computation, AIAA Guidance 
 *       and Control Conference. August 1978. (http://www.malcolmdshuster.com/Pub_1978b_C_PaloAlto_scan.pdf)
 *     - ahrs quest - https://github.com/Mayitzin/ahrs/blob/master/ahrs/filters/quest.py
 * 
 * @param ad_vectors: pointer to struct of input vectors
 * @param weights: array (size 2) of the two relative weights of the measurements, weights should add up to 1.0 (this is not checked)
 *                 (mag: weights[0], sun: weights[1])
 * @param out_quat: pointer to store output: estimated attitude quaternion
 * @return ADCS_SUCCESS if no error, error code otherwise
 * 
 * NOTES:
 *      - Newton method is used, and this may not converge (such that error is <= NEWTON_ITER_TOLERANCE).
 *        In this case, the iterations terminate at NEWTON_ITER_LIMIT.
 */
adcs_err_t quest_estimate(const adcs_ad_triax_vectors_t *ad_vectors, float32_t* weights, float32_t *out_quat) {

    //attitude profile matrix data
    float32_t B_data[3][3] = {0.0f};
    float32_t S_data[3][3] = {0.0f};

    //init matrices S and B as zero
    arm_matrix_instance_f32 S = {.numRows = 3, .numCols = 3, .pData = (float32_t *)S_data};
    arm_matrix_instance_f32 B = {.numRows = 3, .numCols = 3, .pData = (float32_t *)B_data};

    //temp matrices and vectors as zero
    arm_matrix_instance_f32 temp_mat_1 = {.numRows = 3, .numCols = 3, .pData = &(mat_data[0])};
    arm_matrix_instance_f32 temp_mat_2 = {.numRows = 3, .numCols = 3, .pData = &(mat_data[9])};
    arm_matrix_instance_f32 temp_vec_row = {.numRows = 1, .numCols = 3, .pData = &(mat_data[18])};
    arm_matrix_instance_f32 temp_vec_col = {.numRows = 3, .numCols = 1, .pData = &(out_quat[1])};

    //init all local floats for math clarity
    float32_t sigma = 0.0f;
    float32_t delta = 0.0f;
    float32_t kappa = 0.0f;
    float32_t a = 0.0f;
    float32_t b= 0.0f;
    float32_t c = 0.0f;
    float32_t d = 0.0f;
    float32_t k = 0.0f;
    float32_t lam_0 = 0.0f;
    float32_t alpha = 0.0f;
    float32_t beta = 0.0f;
    float32_t lam = weights[0] + weights[1];

    //normalize mag measurements
    //we only care about direction, not intensity
    normalize((float32_t *)(ad_vectors->mag_obs) , 3);
    normalize((float32_t *)(ad_vectors->mag_ref) , 3);

    //create 3x3 attitude profile matrix B
    //magnetic field vectors
    const arm_matrix_instance_f32 mag_b = {.numRows = 3, .numCols = 1, .pData= (float32_t *)(ad_vectors->mag_obs)};
    const arm_matrix_instance_f32 mag_rT = {.numRows = 1, .numCols = 3, .pData= (float32_t *)(ad_vectors->mag_ref)};

    arm_mat_mult_f32(&mag_b, &mag_rT, &temp_mat_1);
    arm_mat_scale_f32(&temp_mat_1, weights[0], &B);

    //sun vectors
    const arm_matrix_instance_f32 sun_b = {.numRows = 3, .numCols = 1, .pData= (float32_t *)(ad_vectors->sun_obs)};
    const arm_matrix_instance_f32 sun_rT = {.numRows = 1, .numCols = 3, .pData= (float32_t *)(ad_vectors->sun_ref)};
    arm_mat_mult_f32(&sun_b, &sun_rT, &temp_mat_1);
    arm_mat_scale_f32(&temp_mat_1, weights[0], &temp_mat_2);
    arm_mat_add_f32(&B, &temp_mat_2, &temp_mat_1);

    //now copy over the data to B, B should now be complete
    memcpy(B.pData, temp_mat_1.pData, 9*sizeof(B_data[0][0]));

    //create S = B + Btranspose
    arm_mat_trans_f32(&B, &temp_mat_1);
    arm_mat_add_f32(&B, &temp_mat_1, &S);

    //create z and z_T
    const float32_t z_data[3] = {B_data[1][2] - B_data[2][1], B_data[2][0] - B_data[0][2], B_data[0][1] - B_data[1][0]};
    const arm_matrix_instance_f32 z = {.numRows = 3, .numCols = 1, .pData = (float32_t *)z_data};
    const arm_matrix_instance_f32 z_T = {.numRows = 1, .numCols = 3, .pData = (float32_t *)z_data};

    //sigma = tr(B)
    sigma = trace3x3(&B);

    //delta = det(S)
    delta = det3x3(&S);

    //kappa = delta * tr(inv(S))
    //for some reason inverse modifies the source arr
    //copy S to temp mat first
    memcpy(temp_mat_1.pData, S.pData, 9*sizeof(S_data[0][0]));

    arm_status mat_status = arm_mat_inverse_f32(&temp_mat_1, &temp_mat_2);

    //check that the inverse could be calculated successfully
    //otherwise return an adcs calculation error
    if (mat_status != ARM_MATH_SUCCESS ) {
        return ADCS_MATH_ERROR;
    }

    kappa = delta * trace3x3(&temp_mat_2);

    //a = sigma^2 - kappa
    a = (sigma * sigma) - kappa;

    //b = sigma^2 + z.z
    b = (sigma * sigma) + dot3(z_data, z_data);

    //c = delta + z_transpose*S*z
    arm_mat_mult_f32(&z_T, &S, &temp_vec_row); // z x S -> temp_vec_row
    c = delta + dot3(temp_vec_row.pData, z.pData);

    //d = z_transpose*S*S*z
    arm_mat_mult_f32(&S, &S, &temp_mat_1); // S x S -> temp_mat_1
    arm_mat_mult_f32(&z_T, &temp_mat_1, &temp_vec_row); // z_transpose x S x S -> tempvecrow
    d = dot3(temp_vec_row.pData, z.pData);

    // k = a*b + c*sigma -d
    k = (a * b) + (c * sigma) - d;

    //Newton-Raphson method with tolerance of 10e-10 and max iterations of 10
    uint8_t iterations = 0;
    while ((fabsf(lam-lam_0) >= NEWTON_ITER_TOLERANCE) && iterations < NEWTON_ITER_LIMIT) {
        lam_0 = lam;
        lam = lam - ((((((lam * lam) - (a + b)) * lam) - c) * lam) + k)/((2.0f * (((2.0f * lam * lam) - (a + b)) * lam)) - c);
        iterations++;
    }
    
    //check convergence condition and return error if not met
    if (iterations >= NEWTON_ITER_LIMIT) {
        return ADCS_MATH_ERROR;
    }

    alpha = ((lam * lam) - (sigma * sigma)) + kappa;
    beta = lam - sigma;

    //now calculuate the quaternion axis. X = (alpha*I + beta*S + S*S)*z
    arm_mat_scale_f32(&S, beta, &temp_mat_2); //beta*S -> temp_mat_2
    //beta*S + alpha*I -> temp_mat_2
    temp_mat_2.pData[0]+=alpha;
    temp_mat_2.pData[4]+=alpha;
    temp_mat_2.pData[8]+=alpha;
    arm_mat_add_f32(&temp_mat_1, &temp_mat_2, &temp_mat_2);   //(beta*S + alpha*I) + S*S -> temp_mat_2
    arm_mat_mult_f32(&temp_mat_2, &z, &temp_vec_col);         //(alpha*I + beta*S + S*S)*z -> x = temp_mat_1

    //this is the quaternion roation angle
    out_quat[0] = (alpha * (lam + sigma)) - delta;

    //normalize
    quaternion_normalize(out_quat);

    return ADCS_SUCCESS;
}

/******************************************************************************/
/*                      P R I V A T E  F U N C T I O N S                      */
/******************************************************************************/

//this is only defined for square matrices
//sum of matrix diagonal for a 3x3 matrix
static float32_t trace3x3(const arm_matrix_instance_f32 *mat) {
    return mat->pData[0] + mat->pData[4] + mat->pData[8];
}

static float32_t det3x3(const arm_matrix_instance_f32 *mat) {
    float32_t *m = mat->pData;
    return   (m[0] * ((m[4] * m[8]) - (m[7] * m[5])))
           - (m[1] * ((m[3] * m[8]) - (m[6] * m[5])))
           + (m[2] * ((m[3] * m[7]) - (m[6] * m[4])));
}

//normalize a vector of size n
static void normalize(float32_t *vec, uint8_t n) {
    float32_t sqrSum = 0.0;
    for(int i = 0; i < n; i++) {
        sqrSum += vec[i] * vec[i];
    }

    float32_t norm;
    arm_sqrt_f32(sqrSum, &norm);

    for(int i = 0; i < n; i++) {
        vec[i] /= norm;
    }
}

//3d dot product
//assumes float arrays are of size 3 (does not check size).
static float32_t dot3(const float32_t *v1, const float32_t *v2) {
    return ((v1[0]*v2[0]) + (v1[1]*v2[1]) + (v1[2]*v2[2]));
}
