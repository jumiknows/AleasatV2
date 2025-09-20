A list of known issues.
Aleasim v0.1.

## Approximations (that can be improved)
- [ ] ECI <-> ECEF transformation only accounts for sidreal rotation and not nutation/precession. This is first order approximate which is probably okay as we don't propagate far from the date, but we should note it down and/or consider using skyfield instead.
- [x] kernel.time and kernel.date do not account for leap seconds, only utc seconds are incremented. kernel.skyfield_time does account for leap seconds and is used for orbit , sun and earth position calculations. At the moment this is ok because time was not used anywhere else. However, this will need to change. A solution could be to use skyfield time to update simulation time and datetime by adding leap seconds when needed.
    - kernel now updates skyfield time internally which accounts for leapseconds
- [ ] We should check to see if we can optimize numerical integration even further by implementing our own in-line RK4 optimized for a specific state derivative (acubesat does this).

## Potential Bugs
- [x] Frame Transformations - there seems to be an issue with the unit test where composing the frame transformations manually (matrix multiplication) does not agree with the get_transformation_to function. Need to look into this.
    - Resolved in commit 7451b8f62818b17201301f4be07894509ffaf750
- [ ] Consider using au instead of m as the base unit for reference frames due to the very large distances involved
    - The accumilated error of chaining multiple transform matrices using [m] as base unit seems to be at the first decimal place (so tens of centimeters)

## Performance

See `demos/profiler_test.py` to look at how time is pofiled for a similation scenario.
It seems that the most time consuming function calls are those related to epherimis (sun, earth positions) handled by `jplephem` and used through `skyfield`.
Pygeomag comes in second.
Interestingly enough the from_dcm method consumes more total time than the attitude dynamics state_update_fcn.
Optimizations can probably be made for pygeomag... not sure about skyfield.

```bash
    8416198 function calls (8253909 primitive calls) in 10.566 seconds

   Ordered by: internal time
   List reduced from 952 to 50 due to restriction <50>

   ncalls  tottime  percall  cumtime  percall filename:lineno(function)
    66000    2.102    0.000    2.373    0.000 /home/yelwishahy/.local/share/virtualenvs/aleasim-uAL0RtV1/lib/python3.10/site-packages/jplephem/spk.py:197(generate)
     3500    0.730    0.000    0.836    0.000 /home/yelwishahy/.local/share/virtualenvs/aleasim-uAL0RtV1/lib/python3.10/site-packages/pygeomag/geomag.py:404(calculate)
64547/43533    0.522    0.000    0.666    0.000 /home/yelwishahy/.local/share/virtualenvs/aleasim-uAL0RtV1/lib/python3.10/site-packages/numpy/_core/numeric.py:1172(roll)
   155538    0.291    0.000    0.291    0.000 {method 'reduce' of 'numpy.ufunc' objects}
122104/10006    0.217    0.000    0.597    0.000 /home/yelwishahy/.local/share/virtualenvs/aleasim-uAL0RtV1/lib/python3.10/site-packages/numpy/_core/arrayprint.py:810(recurser)
    10006    0.205    0.000    0.655    0.000 /home/yelwishahy/.local/share/virtualenvs/aleasim-uAL0RtV1/lib/python3.10/site-packages/numpy/_core/arrayprint.py:961(fillFormat)
    22004    0.178    0.000    0.178    0.000 /home/yelwishahy/.local/share/virtualenvs/aleasim-uAL0RtV1/lib/python3.10/site-packages/skyfield/timelib.py:1042(tdb_minus_tt)
    10001    0.168    0.000    0.427    0.000 /home/yelwishahy/.local/share/virtualenvs/aleasim-uAL0RtV1/lib/python3.10/site-packages/numpy/_core/numeric.py:1512(cross)
     6000    0.158    0.000    0.288    0.000 /home/yelwishahy/.local/share/virtualenvs/aleasim-uAL0RtV1/lib/python3.10/site-packages/skyfield/relativity.py:116(_add_deflection)
     2002    0.145    0.000    0.215    0.000 /home/yelwishahy/.local/share/virtualenvs/aleasim-uAL0RtV1/lib/python3.10/site-packages/skyfield/nutationlib.py:236(iau2000a)
    96079    0.143    0.000    0.197    0.000 /home/yelwishahy/.local/share/virtualenvs/aleasim-uAL0RtV1/lib/python3.10/site-packages/numpy/_core/arrayprint.py:1047(__call__)
    15011    0.142    0.000    0.729    0.000 /home/yelwishahy/aleasat/obc2-firmware/aleasim/aleasim/math_lib/math_scipy.py:51(from_dcm)
     8000    0.134    0.000    0.238    0.000 /home/yelwishahy/aleasat/obc2-firmware/aleasim/aleasim/epa/attitude_dynamics.py:170(state_update_fcn)
   103539    0.128    0.000    0.200    0.000 /home/yelwishahy/.local/share/virtualenvs/aleasim-uAL0RtV1/lib/python3.10/site-packages/numpy/_core/numeric.py:1373(normalize_axis_tuple)
   102021    0.128    0.000    0.128    0.000 {built-in method builtins.divmod}
    16016    0.125    0.000    0.155    0.000 /home/yelwishahy/aleasat/obc2-firmware/aleasim/aleasim/kernel/frames.py:68(_create_transformation_matrix_inverse)
   110562    0.118    0.000    0.118    0.000 {built-in method numpy.array}
    55021    0.109    0.000    0.242    0.000 /home/yelwishahy/.local/share/virtualenvs/aleasim-uAL0RtV1/lib/python3.10/site-packages/skyfield/functions.py:50(length_of)
    27511    0.101    0.000    0.101    0.000 {built-in method numpy._core._multiarray_umath.c_einsum}
    22519    0.100    0.000    0.408    0.000 /home/yelwishahy/aleasat/obc2-firmware/aleasim/aleasim/math_lib/math_scipy.py:21(_from_quat)
   148102    0.087    0.000    0.087    0.000 {built-in method numpy._core._multiarray_umath.dragon4_scientific}
     2000    0.087    0.000    2.104    0.001 /home/yelwishahy/.local/share/virtualenvs/aleasim-uAL0RtV1/lib/python3.10/site-packages/skyfield/relativity.py:22(add_deflection)
1210419/1210413    0.077    0.000    0.077    0.000 {built-in method builtins.len}
    30003    0.076    0.000    0.225    0.000 /home/yelwishahy/.local/share/virtualenvs/aleasim-uAL0RtV1/lib/python3.10/site-packages/numpy/_core/numeric.py:1437(moveaxis)
     4501    0.075    0.000    0.188    0.000 /home/yelwishahy/.local/share/virtualenvs/aleasim-uAL0RtV1/lib/python3.10/site-packages/skyfield/toposlib.py:260(_compute_latitude)
    32012    0.068    0.000    0.068    0.000 {method 'dot' of 'numpy.ndarray' objects}
    33032    0.067    0.000    0.067    0.000 /home/yelwishahy/aleasat/obc2-firmware/aleasim/aleasim/math_lib/math_scipy.py:44(to_DCM)
    19513    0.066    0.000    0.412    0.000 /home/yelwishahy/aleasat/obc2-firmware/aleasim/aleasim/math_lib/math_scipy.py:17(_as_quat)
    96079    0.062    0.000    0.083    0.000 /home/yelwishahy/.local/share/virtualenvs/aleasim-uAL0RtV1/lib/python3.10/site-packages/numpy/_core/arrayprint.py:760(_extendLine)
     1000    0.062    0.000    0.332    0.000 /home/yelwishahy/aleasat/obc2-firmware/aleasim/aleasim/epa/disturbance_model.py:83(solar_pressure)
    22000    0.060    0.000    2.511    0.000 /home/yelwishahy/.local/share/virtualenvs/aleasim-uAL0RtV1/lib/python3.10/site-packages/skyfield/jpllib.py:215(_at)
    22000    0.057    0.000    2.431    0.000 /home/yelwishahy/.local/share/virtualenvs/aleasim-uAL0RtV1/lib/python3.10/site-packages/jplephem/spk.py:160(compute_and_differentiate)
    96079    0.057    0.000    0.161    0.000 /home/yelwishahy/.local/share/virtualenvs/aleasim-uAL0RtV1/lib/python3.10/site-packages/numpy/_core/arrayprint.py:774(_extendLine_pretty)
    19000    0.054    0.000    0.093    0.000 /home/yelwishahy/.local/share/virtualenvs/aleasim-uAL0RtV1/lib/python3.10/site-packages/numpy/linalg/_linalg.py:2566(norm)
     6005    0.050    0.000    0.165    0.000 /home/yelwishahy/.local/share/virtualenvs/aleasim-uAL0RtV1/lib/python3.10/site-packages/skyfield/timelib.py:183(_utc)
    14000    0.047    0.000    0.047    0.000 {built-in method time.mktime}
    82054    0.045    0.000    0.089    0.000 /home/yelwishahy/.local/share/virtualenvs/aleasim-uAL0RtV1/lib/python3.10/site-packages/numpy/_core/arrayprint.py:987(<genexpr>)
    56060    0.044    0.000    0.108    0.000 {built-in method builtins.max}
   133550    0.043    0.000    0.068    0.000 /home/yelwishahy/.local/share/virtualenvs/aleasim-uAL0RtV1/lib/python3.10/site-packages/skyfield/functions.py:158(_to_array)
    16016    0.043    0.000    0.092    0.000 /home/yelwishahy/aleasat/obc2-firmware/aleasim/aleasim/kernel/frames.py:60(_create_transformation_matrix)
17004/9004    0.042    0.000    0.661    0.000 /home/yelwishahy/aleasat/obc2-firmware/aleasim/aleasim/kernel/frames.py:177(get_transformation_to)
     8000    0.042    0.000    0.595    0.000 /home/yelwishahy/aleasat/obc2-firmware/aleasim/aleasim/kernel/frames.py:123(__mul__)
    26510    0.041    0.000    0.138    0.000 /home/yelwishahy/.local/share/virtualenvs/aleasim-uAL0RtV1/lib/python3.10/site-packages/numpy/_core/fromnumeric.py:69(_wrapreduction)
     1000    0.041    0.000    4.637    0.005 /home/yelwishahy/aleasat/obc2-firmware/aleasim/aleasim/epa/attitude_dynamics.py:122(update)
    22002    0.038    0.000    0.081    0.000 /home/yelwishahy/.local/share/virtualenvs/aleasim-uAL0RtV1/lib/python3.10/site-packages/skyfield/positionlib.py:95(__init__)
    10006    0.037    0.000    1.398    0.000 /home/yelwishahy/.local/share/virtualenvs/aleasim-uAL0RtV1/lib/python3.10/site-packages/numpy/_core/arrayprint.py:539(_array2string)
    78547    0.037    0.000    0.037    0.000 {built-in method numpy.zeros}
     1000    0.036    0.000    0.219    0.000 /home/yelwishahy/.local/share/virtualenvs/aleasim-uAL0RtV1/lib/python3.10/site-packages/scipy/integrate/_ivp/rk.py:14(rk_step)
     2000    0.036    0.000    0.880    0.000 /home/yelwishahy/.local/share/virtualenvs/aleasim-uAL0RtV1/lib/python3.10/site-packages/skyfield/vectorlib.py:224(_correct_for_light_travel_time)
   103539    0.034    0.000    0.052    0.000 /home/yelwishahy/.local/share/virtualenvs/aleasim-uAL0RtV1/lib/python3.10/site-packages/numpy/_core/numeric.py:1424(<listcomp>)
```
