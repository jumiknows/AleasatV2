import numpy as np
from alea.sim.math_lib import skew

def torque_allocation_qp(torque_desired: np.ndarray, b:np.ndarray, weights=None, mask=None) -> tuple[np.ndarray, np.ndarray]:
    """
    Distribute desired torque vector [Nm] between 3 reaction wheels and 3 magnetorquers on the pricipal axes
    Returns reaction wheel torques in Nm
    Returns magnetic moments in Am^2
    """
    m = np.zeros([3,])
    tw = np.zeros(3)

    if weights is None:
        weights = np.ones(6)

    Qqp = np.diag(weights)
    assert Qqp.shape == (6,6)
    Aqp = np.concatenate([skew(b), np.eye(3)],axis=1)
    assert Aqp.shape == (3,6)

    if mask is not None:
        Qqp = np.diag(weights[mask.astype('bool')])
        Aqp = Aqp[:,mask.astype('bool')]
        n = mask.sum()
        M = np.zeros((n+3,n+3))
        M[:n,:n] = Qqp
        M[n:n+3, :n] = Aqp
        M[:n, n:n+3] = Aqp.T
        v = np.zeros(n+3)
        v[n:n+3] = torque_desired
    else:
        M = np.zeros((9,9))
        M[:6,:6] = Qqp
        M[6:9, :6] = Aqp
        M[:6, 6:9] = Aqp.T
        v = np.zeros(9)
        v[6:9] = torque_desired

    sol = np.linalg.inv(M) @ v

    if mask is not None:
        u = np.zeros(6)
        u[mask] = sol[:n]
    else:
        u = sol[:6]
    
    m = u[:3]
    tw = u[3:6]
    
    return tw, m

def simple_torque_allocation(torque_desired: np.ndarray, b:np.ndarray, weights:np.ndarray=None) -> tuple[np.ndarray, np.ndarray]:
    """
    Distribute desired torque vector [Nm] between 3 reaction wheels and 3 magnetorquers on the pricipal axes.
    weights are [mx, my, mz, rwx, rwy, rwz]
    if weights = None, defaults to np.ones(6)

    Args:
        torque_desire: Desired body torque vector in Nm

        b: Body frame magnetic field vector [X, Y, Z] in Tesla

        weights (optional): Weight values ranging -1.0 to 1.0, weights are ordered [mtqx, mtqy, mtqz, rwx, rwy, rwz]

    Returns:
        Tuple of reaction wheel torques [Nm] and magnetroquer moments [A-m^2]
    """
    m = np.zeros(3)
    rw = np.zeros(3)

    if weights is None:
        weights = np.ones(6)
        
    rw = weights[3:6] * torque_desired
    remaining = torque_desired - rw
    m = weights[0:3] * (np.linalg.pinv(-1.0*skew(b)) @ remaining)

    return rw, m