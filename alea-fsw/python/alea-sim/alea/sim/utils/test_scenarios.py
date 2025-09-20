from alea.sim.kernel.kernel import AleasimKernel
from alea.sim.kernel.root_models import AleasimRootModel

def create_aleasim_test_kernel() -> AleasimKernel:
    """Setup kernel scenario for test cases"""
    dt = 0.01
    kernel = AleasimKernel(dt=dt, date=2024.2)
    root = AleasimRootModel(kernel)
    kernel.add_model(root)
    return kernel