import numpy as np

def calculate_atmospheric_density(altitude: float, 
                                  h0: float, 
                                  rho0: float, 
                                  H: float) -> float:
    """
    Calculate atmospheric density using a simple exponential model.
    This model is adequate for preliminary CubeSat simulations.
    # TODO - future work - There are more complex models that are better approximations
    
    Based on Appendix D.2.1 of Markley & Crassidis, the exponential model is:
        ρ = ρ₀ * exp[-(h - h₀) / H]
        
    Use Table D.1 of Markley & Crassidis for values: h0, rho0 (p0), and H.
    
    Parameters:
      altitude : float
          The altitude above Earth's surface (in meters).
      h0 : float
          The reference altitude in meters
      rho0 : float
          The density at the reference altitude in kg/m³
      H : float, optional
          The scale height in meters

    Returns:
      float
          The estimated atmospheric density in kg/m³.
    """
    density = rho0 * np.exp(-(altitude - h0) / H)
    return density