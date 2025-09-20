import unittest

from alea.sim.kernel.time_cached import TimeCachedModel, time_cached_property

GLOBAL_TIME = 0

class TestModel(TimeCachedModel):
    def __init__(self):
        super().__init__()
        self.call_count1 = 0
        self.call_count2 = 0

    @property
    def current_time(self) -> int:
        global GLOBAL_TIME
        return GLOBAL_TIME

    @time_cached_property
    def property1(self) -> int:
        """This property will be updated from the update function, so the function here
        won't actually be used"""
        self.call_count1 += 1
        return int(self.current_time)

    @time_cached_property
    def property2(self) -> int:
        """This property will only be populated by the function here"""
        self.call_count2 += 1
        return int(self.current_time)

    def update(self):
        self.invalidate_cache(self.current_time)
        self.property1 = -1 * int(self.current_time)

class TimeCachedTest(unittest.TestCase):
    def setUp(self):
        global GLOBAL_TIME
        GLOBAL_TIME = 0
        self.model = TestModel()

    def step(self):
        global GLOBAL_TIME
        GLOBAL_TIME += 1
        self.model.update()

    def test_property1(self):
        """
        property1 is written from the update function so call_count1 should always be 0
        and the value of the property should be -1 * time
        """

        self.assertEqual(self.model.call_count1, 0)
        self.step()
        self.assertEqual(self.model.call_count1, 0)

        value = self.model.property1
        self.assertEqual(value, -1)
        self.assertEqual(self.model.call_count1, 0)

        # Read the property again and make sure the call_count1 doesn't change
        value = self.model.property1
        self.assertEqual(value, -1)
        self.assertEqual(self.model.call_count1, 0)

        # Advance the simulation and make sure the value of the property is updated
        # without call_count1 changing
        self.step()
        value = self.model.property1
        self.assertEqual(value, -2)
        self.assertEqual(self.model.call_count1, 0)

    def test_property2(self):
        """
        property2 is written on-the-fly so we should see call_count2 increment every
        time it's regenerated (only when time increments)
        """
        self.assertEqual(self.model.call_count2, 0)
        self.step()
        self.assertEqual(self.model.call_count2, 0)

        # Read the property once and make sure the call_count2 increments
        value = self.model.property2
        self.assertEqual(value, 1)
        self.assertEqual(self.model.call_count2, 1)

        # Read the property again and make sure the call_count2 doesn't change
        value = self.model.property2
        self.assertEqual(value, 1)
        self.assertEqual(self.model.call_count2, 1)

        # Advance the simulation and make sure the value of the property is updated
        # only after reading the property
        self.step()
        self.assertEqual(self.model.call_count2, 1)
        value = self.model.property2
        self.assertEqual(value, 2)
        self.assertEqual(self.model.call_count2, 2)
