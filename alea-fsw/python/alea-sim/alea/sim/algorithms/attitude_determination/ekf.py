import numpy as np

"""
    EKF Estimator for the attitude dynamics of an inertial pointing spacecraft

    Based on section 8.1 of Yang, Yaguang - Spacecraft modeling, attitude determination, 
    and control_ quaternion-based approach
    
"""


class ExtendedKalmanFilter:
    def __init__(self, J, dt, Q, R):
        """
        Initialize the EKF with system-specific parameters.
        
        Parameters:
        - J: The inertia matrix (numpy array).
        - dt: The sampling time period.
        - Q: The process noise covariance matrix (numpy array).
        - R: The measurement noise covariance matrix (numpy array).
        """
        self.J = J
        self.dt = dt
        self.Q = Q
        self.R = R

    def F_func(self, x, u):
        """
        State transition function F(x, u).
        """
        omega_k = x[:3]
        q_k = x[3:6]
        beta_k = x[6:]

        # Compute the next state components Eq. (8.6a)
        omega_k1 = omega_k + (-np.linalg.inv(self.J) @ np.cross(omega_k, self.J @ omega_k) + np.linalg.inv(self.J) @ u) * self.dt
        Omega_k = np.array([
            [np.sqrt(1 - q_k[0]**2 - q_k[1]**2 - q_k[2]**2), -q_k[2], q_k[1]],
            [q_k[2], np.sqrt(1 - q_k[0]**2 - q_k[1]**2 - q_k[2]**2), -q_k[0]],
            [-q_k[1], q_k[0], np.sqrt(1 - q_k[0]**2 - q_k[1]**2 - q_k[2]**2)]
        ])
        q_k1 = q_k + 0.5 * Omega_k @ omega_k * self.dt
        beta_k1 = beta_k

        # Concatenate the state vector Eq. (8.6b)
        x_k1 = np.concatenate([omega_k1, q_k1, beta_k1])
        
        return x_k1

    def F_jacobian(self, x, u):
        """
        Jacobian of the state transition function F with respect to the state x.
        """
        omega_k = x[:3]
        q_k = x[3:6]

        # Compute the First element in the matrix Eq. (8.11)
        F11 = np.eye(3) - np.linalg.inv(self.J) @ (np.cross(omega_k, self.J) - np.cross(self.J @ omega_k, np.eye(3))) * self.dt

        # Compute the Jacobians Eq. (8.7)'
        Omega_k = np.array([
            [np.sqrt(1 - q_k[0]**2 - q_k[1]**2 - q_k[2]**2), -q_k[2], q_k[1]],
            [q_k[2], np.sqrt(1 - q_k[0]**2 - q_k[1]**2 - q_k[2]**2), -q_k[0]],
            [-q_k[1], q_k[0], np.sqrt(1 - q_k[0]**2 - q_k[1]**2 - q_k[2]**2)]
        ])

        g_q = np.sqrt(1 - q_k[0]**2 - q_k[1]**2 - q_k[2]**2)
        
        # Compute the gradient of F2 to omega Eq. (8.9)
        F2omega_jacobian = 0.5 * Omega_k * self.dt

        # Compute the gradient of F2 to q Eq. (8.10)
        dF2_dq = np.array([
            [1/self.dt - (q_k[0] * omega_k[0]) / (2 * g_q), (omega_k[2] / 2) - (q_k[1] * omega_k[0]) / (2 * g_q), -(omega_k[1] / 2) - (q_k[2] * omega_k[0]) / (2 * g_q)],
            [-(omega_k[2] / 2) - (q_k[0] * omega_k[1]) / (2 * g_q), 1/self.dt - (q_k[1] * omega_k[1]) / (2 * g_q), (omega_k[0] / 2) - (q_k[2] * omega_k[1]) / (2 * g_q)],
            [(omega_k[1] / 2) - (q_k[0] * omega_k[2]) / (2 * g_q), -(omega_k[0] / 2) - (q_k[1] * omega_k[2]) / (2 * g_q), 1/self.dt - (q_k[2] * omega_k[2]) / (2 * g_q)]
        ]) 

        F2q_jacobian = dF2_dq * self.dt

        # Construct the full Jacobian matrix with Eq (8.11)
        F_jacobian_matrix = np.block([
            [F11, np.zeros((3, 3)), np.zeros((3, 3))],
            [F2omega_jacobian, F2q_jacobian, np.zeros((3, 3))],
            [np.zeros((3, 3)), np.zeros((3, 3)), np.eye(3)]
        ])

        return F_jacobian_matrix

    def H_func(self, x):
        """
        Measurement function H(x).
        """
        omega_k = x[:3]
        q_k = x[3:6]
        beta_k = x[6:]
        
        # Concatenate omega and q for the measurement Eq (8.6b)
        y_k = np.concatenate([omega_k+beta_k, q_k])
        
        return y_k

    def predict(self, x_prev, P_prev, u_prev):
        """
        Perform the prediction step of the EKF.
        """

        omega_k_prev = x_prev[:3]
        q_k_prev = x_prev[3:6]
        beta_k_prev  = x_prev[6:]

        # Predict state estimate (8.13a)
        x_pred = self.F_func(x_prev, u_prev)
        
        # Predict state covariance (8.13b)
        F_km1 = self.F_jacobian(x_prev, u_prev)

        # Find Omega_k_prev for P (8.13b)
        Omega_k_prev = np.array([
            [np.sqrt(1 - q_k_prev[0]**2 - q_k_prev[1]**2 - q_k_prev[2]**2), -q_k_prev[2], q_k_prev[1]],
            [q_k_prev[2], np.sqrt(1 - q_k_prev[0]**2 - q_k_prev[1]**2 - q_k_prev[2]**2), -q_k_prev[0]],
            [-q_k_prev[1], q_k_prev[0], np.sqrt(1 - q_k_prev[0]**2 - q_k_prev[1]**2 - q_k_prev[2]**2)]
        ])

        # Find L (8.12)
        L_k = np.block([
            [ np.eye(3)* self.dt, np.zeros((3, 3)), np.zeros((3, 3))],
            [np.zeros((3, 3)), 0.5*Omega_k_prev * self.dt, np.zeros((3, 3))],
            [np.zeros((3, 3)), np.zeros((3, 3)),  np.eye(3)* self.dt]
        ])

        # Find prediction of P (8.13b)
        P_pred = F_km1 @ P_prev @ F_km1.T + L_k @ self.Q @ L_k.T
        
        return x_pred, P_pred

    def update(self, x_pred, P_pred, y_k):
        """
        Perform the update step of the EKF.
        """
        # Compute the innovation (8.13c)
        H_k = np.block([
            [np.eye(3), np.zeros((3, 3)), np.zeros((3, 3))],
            [np.zeros((3, 3)), np.eye(3), np.zeros((3, 3))]
        ])
        y_tilde = y_k - self.H_func(x_pred)
        
        # Compute the innovation covariance (8.13d)
        S_k = H_k @ P_pred @ H_k.T + self.R
        
        # Compute the Kalman gain (8.13e)
        K_k = P_pred @ H_k.T @ np.linalg.inv(S_k)
        
        # Update the state estimate (8.13f)
        x_upd = x_pred + K_k @ y_tilde
        
        # Update the state covariance matrix (8.13g)
        P_upd = (np.eye(len(x_pred)) - K_k @ H_k) @ P_pred
        
        return x_upd, P_upd
