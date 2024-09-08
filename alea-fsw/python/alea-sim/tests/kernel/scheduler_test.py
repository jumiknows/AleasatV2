import unittest
from alea.sim.kernel.scheduler import Scheduler

class SchedulerTest(unittest.TestCase):

    def _time_func(self):
        return self.time

    def _delay_func(self, dt:int):
        self.time = self.time + dt

    def action1(self):
        self.cnt1+=1
        print(f'action1 at time {self._time_func()}')

    def action2(self):
        self.cnt2+=1
        print(f'action2 at time {self._time_func()}')

    def test_scheduler_1(self):
        self.time = 0
        self.cnt1 = 0
        self.cnt2 = 0

        scheduler = Scheduler(self._time_func, self._delay_func)
        
        scheduler.enter(0, 1, self.action1, 1)
        scheduler.enter(10, 0, self.action2)
        
        #run until current_time >= start_time + 5
        res = scheduler.run(5)
        assert self.cnt1 == 5
        assert self._time_func() == 4
        assert res == 1
        
        res = scheduler.run(7)

    def test_scheduler_1(self):
        self.time = 0
        self.cnt1 = 0
        self.cnt2 = 0

        scheduler = Scheduler(self._time_func, self._delay_func)
        
        scheduler.enter(0, 1, self.action1, 1)
        scheduler.enter(10, 0, self.action2)
        
        #run until current_time >= start_time + 5
        res = scheduler.run_until_empty(blocking=False)
        assert self.cnt1 == 1
        assert self._time_func() == 0
        assert res == 1
        
        self._delay_func(res)

        res = scheduler.run_until_empty(blocking=False)
        assert self.cnt1 == 2
        assert self._time_func() == 1
        assert res == 1

if __name__ == '__main__':
    unittest.main()