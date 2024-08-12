"""
Python Implementations of well known solutions to Wahba's problem. https://en.wikipedia.org/wiki/Wahba%27s_problem
Includes implementations of Quest, Davenport, and SVD among others.

Only Quest and Davenport have been fully tested here.

Author: Yousif El-Wishahy 
    
ALEASAT 2024

References -
    see function docs
"""

import numpy as np
from aleasim.math_lib import Quaternion, normalize

def attitude_profile_matrix(vec_obs:np.ndarray ,vec_ref: np.ndarray, w:np.ndarray=None) -> np.ndarray:
    B:np.ndarray = np.zeros([3,3])
    for i in range(len(w)):
        B += w[i]*np.outer(normalize(vec_obs[i]), normalize(vec_ref[i]))
    return B

def wahba_davenport(vec_obs:np.ndarray ,vec_ref: np.ndarray, w:np.ndarray=None) -> Quaternion:
    """
    Implementation of solution described in
    [Davenport1968]	Paul B. Davenport. A Vector Approach to the Algebra of Rotations with Applications. 
    NASA Technical Note D-4696. August 1968. (https://ntrs.nasa.gov/archive/nasa/casi.ntrs.nasa.gov/19680021122.pdf)
    """
    if vec_obs.shape != vec_ref.shape:
        raise ValueError("Obc and Ref vector arrays have different shapes!")
    if w == None:
        w = np.ones(vec_obs.shape[0])
        w /= np.sum(w)
    
    B = attitude_profile_matrix(vec_obs, vec_ref, w)

    sigma = B.trace()
    z = np.array([B[1, 2]-B[2, 1], B[2, 0]-B[0, 2], B[0, 1]-B[1, 0]])
    S = B+B.T
    K = np.zeros((4, 4))
    K[0, 0] = sigma
    K[1:, 1:] = S - sigma*np.eye(3)
    K[0, 1:] = K[1:, 0] = z
    w, v = np.linalg.eig(K)
    return Quaternion(v[:, np.argmax(w)])

def wahba_quest(vec_obs:np.ndarray ,vec_ref: np.ndarray, w:np.ndarray=None) -> Quaternion:
        """Reference: http://www.malcolmdshuster.com/Pub_1978b_C_PaloAlto_scan.pdf"""
        if vec_obs.shape != vec_ref.shape:
            raise ValueError("Obc and Ref vector arrays have different shapes!")
        if w is None:
            w = np.ones(vec_obs.shape[0])
            w /= np.sum(w)

        B = attitude_profile_matrix(vec_obs, vec_ref, w)
        S = B + B.T
        z = np.array([B[1, 2]-B[2, 1], B[2, 0]-B[0, 2], B[0, 1]-B[1, 0]])
        sigma = B.trace()
        Delta = np.linalg.det(S)
        kappa = (Delta*np.linalg.inv(S)).trace()
        a = sigma**2 - kappa
        b = sigma**2 + z@z
        c = Delta + z@S@z
        d = z@S@S@z

        # Newton-Raphson method
        k = a*b + c*sigma - d
        lam = w.sum()
        lam_0 = 0
        i = 0
        while abs(lam-lam_0)/(lam)>=1e-10:
            lam_0 = lam
            lam = lam - (((lam * lam - (a + b)) * lam - c) * lam + k)/(2.0 * (2.0 * lam * lam - (a + b)) * lam - c)
            i+=1
        alpha = lam**2 - sigma**2 + kappa
        beta = lam - sigma
        gamma = alpha*(lam+sigma) - Delta
        X = (alpha*np.eye(3) + beta*S + S@S)@z
        q = np.array([gamma, X[0],X[1],X[2]])
        return Quaternion(q/np.linalg.norm(q))

#TODO fails unit test...
def wahba_svd(vec_obs:np.ndarray ,vec_ref: np.ndarray, w:np.ndarray=None) -> Quaternion:
    """
    References:  
    [1] F. Landis Markley - Attitude Determination using Vector 
        Observations and Singular Value Decomposition (1968)
    """
    if vec_obs.shape != vec_ref.shape:
        raise ValueError("Obc and Ref vector arrays have different shapes!")
    if w == None:
        w = np.ones(vec_obs.shape[0])
        w /= np.sum(w)

    B = attitude_profile_matrix(vec_obs, vec_ref, w)

    U,_,V = np.linalg.svd(B) #eq 6
    
    d = np.linalg.det(U) * np.linalg.det(V) #eq 14
    
    A = U * np.diag([1,1,d]) * np.transpose(V) #eq 18
    
    #A represents the rotation as a direction cosine matrix
    #need to convert to quaternion
    return Quaternion.from_dcm(A)

#TODO fails unit test...
def wahba_oleq(vec_obs:np.ndarray ,vec_ref: np.ndarray, w:np.ndarray=None) -> Quaternion:
    """reference: https://www.mdpi.com/2072-4292/10/3/377"""
    if vec_obs.shape != vec_ref.shape:
        raise ValueError("Obc and Ref vector arrays have different shapes!")
    if w == None:
        w = np.ones(vec_obs.shape[0])
        w /= np.sum(w)
    
    def W_matrix(b:np.ndarray, r:np.ndarray) -> np.ndarray:
        #from a tri-axial reference and observation vector pair
        #creat 4x4 w matrix
        #w matrix defined in eq.18->20 of paper
        bx, by, bz = b
        rx, ry, rz = r
        
        M1 = np.array([
            [bx, 0, bz, -by],
            [0, bx, by, bz],
            [bz, by, -bx, 0],
            [-by, bz, 0, -bx]])

        M2 = np.array([
            [by, -bz, 0, bx],
            [-bz, -by, bx, 0],
            [0, bx, by, bz],
            [bx, 0, bz, -bx]])

        M3 = np.array([
            [bz, by, -bx, 0],
            [by, -bz, 0, bx],
            [-bx, 0, -bz, by],
            [0, bx, by, bz]])
        
        W = rx*M1 + ry*M2 + rz*M3
        return W

    #create 4x4 R matrix (eq33)
    R = np.zeros([4,4])
    for i in range(len(w)):
        R+= 0.5 * w[i] * (W_matrix(vec_obs[i], vec_ref[i]) + np.identity(4))
    
    #starter values of quaternions for interation
    #arbitrary/random
    q = np.array([1.0, 0., 0.,0.]) 
    last_q = np.array([0., 0., 0., 1.])
    i = 0
    while np.linalg.norm(q-last_q) > 1e-10 and i <= 1000:
        last_q = q
        q = R @ last_q                 # (eq. 24)
        q /= np.linalg.norm(q)
        i += 1
    # print(f'oleq finished at iteration {i}')

    q_sol = q/np.linalg.norm(q)
    #flip to scalar,x,y,z
    q_sol = np.array([q_sol[3],q_sol[0],q_sol[1],q_sol[2]])
    return Quaternion(q_sol)

#TODO fails unit test...
def wahba_flae(vec_obs:np.ndarray ,vec_ref: np.ndarray, w:np.ndarray=None) -> Quaternion:
    """
    Referenc https://hal.inria.fr/hal-01513263
    """
    if vec_obs.shape != vec_ref.shape:
        raise ValueError("Obc and Ref vector arrays have different shapes!")
    if w == None:
        w = np.ones(vec_obs.shape[0])
        w /= np.sum(w)
    
    #create H matrix, 3x3 matrices in eq 42
    H = np.zeros([3,3])
    for i in range(len(w)):
        r = normalize(vec_ref[i])
        b = normalize(vec_obs[i])
        H += w[i] * r.T @ b

    #create 4x4 W matrix from eq 45
    #this is disgusting
    #very very disgusting
    W = np.array([
        [H[0,0] + H[1,1] + H[2,2],  -H[1,2] + H[2,1], -H[2,0] + H[0,2],   -H[0,1] + H[1,0]],
        [-H[1,2] + H[2,1],    H[0,0] - H[1,1] - H[2,2], H[0,1] + H[1,0], H[0,2] + H[2,0]],
        [-H[2,0] + H[0,2], H[0,1] + H[1,0], H[1,1]- H[0,0] -H[2,2], H[1,2] + H[2,1]],
        [-H[0,1] + H[1,0], H[0,2] + H[2,0], H[1,2] + H[2,1], H[2,2] - H[1,1] - H[0,0]]
    ])
    
    #eq 49 to get t params for symbolic solution to eigenvectors
    t1 = -2*np.trace(H@H.T)
    t2 = -8*np.linalg.det(H.T)
    t3 = np.linalg.det(W)

    #parameters for eigen value symbolic eqs (eq53)
    T0 = 2*t1**3 + 27*t2**2 - 72*t1*t3
    T1 = np.cbrt(T0 + np.sqrt(abs(-4*(t1**2 + 12*t3)**3 + T0**2)))
    T2 = np.sqrt(abs(-4*t1 + 2**(4/3)*(t1**2 + 12*t3)/T1 + 2**(2/3)*T1))
    
    #eigen values (eq52)
    L = np.zeros(4)
    k1 = -T2**2 - 12*t1
    k2 = 12*np.sqrt(6)*t2/T2
    L[0] =   T2 - np.sqrt(abs(k1 - k2))
    L[1] =   T2 + np.sqrt(abs(k1 - k2))
    L[2] = -(T2 + np.sqrt(abs(k1 + k2)))
    L[3] = -(T2 - np.sqrt(abs(k1 + k2)))
    L /= 2*np.sqrt(6)
    
    # Eigenvalue closest to 1
    lam = L[(np.abs(L-1.0)).argmin()]

    # 4x4 n matrix from eq 54
    N = W - lam*np.identity(4)
    
    #row reduction on N matrix
    for i in range(3):
        N[i] /= N[i, i]
        for j in range(4):
            if i!=j:
                N[j] -= N[j, i]*N[i]
    
    #extract quaternion from reduced N (eq 57,58)
    #in order scalar, vector[x,y,z]
    q = np.array([-1,N[0, 3], N[1, 3], N[2, 3]])
    return Quaternion(q/np.linalg.norm(q))

#TODO fails unit test...
def wahba_yang2013(vec_obs:np.ndarray ,vec_ref: np.ndarray, w:np.ndarray=None) -> Quaternion:
    """
    Analytical solution to the davenport problem, using quartic function solution
    https://arxiv.org/abs/1309.5679
    """
    def solve_quartic_analytic(a,b,c,d,tolerance=10E-3):
        """
        Analytic solution to roots of p(x) = x**4 + ax**3 + bx**2 + cx + d
        
        Proposed in ref paper
        """

        #eq9
        p = a*c - (b**2)/2 - 4*d
        q = a*b*c/3 - (a**2)*d - (2/27)*b**3 - c**2 + (8/3)*b*d
        w1 =  - 1 + 1j*np.sqrt(3)/2
        w2 =  - 1 - 1j*np.sqrt(3)/2

        # Cardano's formula (Eqn 10)
        def cpow(v1,v2):
            return np.power(v1,v2,dtype='complex')
        v1 = cpow((q/2)**2 + (p/3)**3, 1/2)
        term1 = cpow( -1 * q / 2 + v1, 1/3)
        term2 = cpow( -1 * q / 2 - v1, 1/3)
        y1 = term1 + term2
        y2 = w1*term1 + w2*term2
        y3 = w2*term1 + w1*term2

        def is_imag(v):
            return np.abs(np.imag(v)) >= 1E-10
        # Eqn 12 holds for real y
        if is_imag(y1) == 0:
            y = y1
        elif is_imag(y2) == 0:
            y = y2
        elif is_imag(y3) == 0:
            y = y3

        # Eqn 12
        g1 = (y - (2/3)*b)**0.5
        g2 = -1 * (y - (2/3)*b)**0.5
        h1 = (y1 + b/3 + ((y1 + b/3)**2 - 4*d)**0.5)/2
        h2 = (y1 + b/3 - ((y1 + b/3)**2 - 4*d)**0.5)/2

        # Check if g1*h2 + g2*h1 = c holds.
        # If not swap the variables
        if np.abs(g1*h2+g2*h1-c) > tolerance:
            h  = h1; 
            h1 = h2; 
            h2 = h

        # Roots of quartic (Eqn 14)
        x1 = 0.5*(-g1 + (g1**2 - 4*h1)**0.5)
        x2 = 0.5*(-g1 - (g1**2 - 4*h1)**0.5)
        x3 = 0.5*(-g2 + (g2**2 - 4*h2)**0.5)
        x4 = 0.5*(-g2 - (g2**2 - 4*h2)**0.5)

        return (np.array([np.real(x1), np.real(x2), np.real(x3), np.real(x4)])).max()

    if vec_obs.shape != vec_ref.shape:
        raise ValueError("Obc and Ref vector arrays have different shapes!")
    if w == None:
        #distribute even weights to all measurements
        w = np.ones(vec_obs.shape[0])
        w /= np.sum(w)

    #create 4x4 K matrix     (eq3)
    B = attitude_profile_matrix(vec_obs, vec_ref, w)
    trace_B = np.trace(B)
    S = B + B.T 

    z = np.array([B[1,2] - B[2,1], B[2,0] - B[0,2], B[0,1] - B[1,0]])
    
    K = np.zeros((4,4))
    K[0:3, 0:3] = S - trace_B * np.identity(3)
    K[3, 0:3] = z.T
    K[0:3,3] = z
    K[3,3] = trace_B
    
    def adj(mat):
        return np.linalg.det(mat) * np.linalg.inv(mat)
    
    adj_B = adj(B)
    adj_S = adj(S)
    adj_K = adj(K)
    
    #eq 6 for coefficients of quartic eq
    a = 0
    b = -2.0 * trace_B**2 + np.trace(adj_S) - z.T @ z
    c = -1.0 * np.trace(adj_K)
    d = np.linalg.det(K)
    
    lam = solve_quartic_analytic(a,b,c,d)

    # Determine optimal DCM (Markley1993)
    norm_B = np.linalg.norm(B)
    kappa = 0.5*(lam**2 - norm_B**2)
    zeta = kappa*lam - np.linalg.det(B)

    dcm = ((kappa + norm_B**2) * B + lam*adj_B.T - B @ B.T @ B)/zeta
    dcm_trace = np.trace(dcm)

    n = 0.5*np.sqrt(dcm_trace + 1.0, dtype='complex')
    e = np.array([0.5*np.sign(dcm[2, 1]-dcm[1, 2])*np.sqrt(dcm[0, 0]-dcm[1, 1]-dcm[2, 2]+1.0),
                  0.5*np.sign(dcm[0, 2]-dcm[2, 0])*np.sqrt(dcm[1, 1]-dcm[2, 2]-dcm[0, 0]+1.0),
                  0.5*np.sign(dcm[1, 0]-dcm[0, 1])*np.sqrt(dcm[2, 2]-dcm[0, 0]-dcm[1, 1]+1.0)])
    return Quaternion(np.array([n, *e]))

WAHBA_ESTIMATORS = [wahba_davenport, wahba_quest]
