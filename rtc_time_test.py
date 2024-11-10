import pytest
import datetime as dt
from datetime import timedelta

class TimeTester:
    pca2129_numberOfDaysInMonth = [31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31]
    pca2129_cumulativeSecondsInMonth = [0, 2678400, 5097600, 7776000, 10368000, 13046400, 15638400, 18316800,
        20995200, 23587200, 26265600, 28857600]

    def __init__(self, year, month, day, hour, minute, second):
        self.year = year
        self.month = month
        self.day = day
        self.hour = hour
        self.minute = minute
        self.second = second

    def get_future_time(self, timeSec):
        deltaDays = timeSec//(60*60*24)
        timeSec = timeSec%(60*60*24)
        deltaHours = timeSec//(60*60)
        timeSec = timeSec%(60*60)
        deltaMinutes = timeSec//60
        deltaSecs = timeSec%60

        newSecond = (self.second + deltaSecs)%60
        temp_min = (self.second + deltaSecs)//60 + deltaMinutes + self.minute
        newMinute = temp_min%60
        temp_hour = temp_min//60 + deltaHours + self.hour
        newHour = temp_hour%24
        temp_day = temp_hour//24 + deltaDays + self.day
        newYear = self.year

        month_index = self.month - 1
        days_in_month = TimeTester.pca2129_numberOfDaysInMonth[month_index]
        if self.year%4 == 0 and self.month == 2:
            days_in_month = 29

        while temp_day > days_in_month:
            month_index += 1
            if month_index > 11:
                month_index = 0
                newYear += 1

            temp_day -= days_in_month
            days_in_month = TimeTester.pca2129_numberOfDaysInMonth[month_index];
            if newYear%4 == 0 and month_index == 1:
                days_in_month = 29


        newDay = temp_day
        newMonth = month_index + 1
        return {'year': newYear, 'month': newMonth, 'day': newDay,
                'hour': newHour, 'minute': newMinute, 'second' :newSecond}

    def pca2129_getCurrentRTCTime(self):
        pca2129_het_epoch_time = 0;

        monthIdx = self.month - 1;
        if (monthIdx >= 12):
            monthIdx = 11
        
        nonLeapYear = (self.year * 31536000) + (TimeTester.pca2129_cumulativeSecondsInMonth[monthIdx]) + ((self.day - 1) * 86400) + (self.hour * 3600) + (self.minute * 60) + self.second;

        pca2129_rtc_epoch_time = nonLeapYear;

        if (self.month >= 3 and self.year == 0) or (self.year >> 0):
            pca2129_rtc_epoch_time += 86400
        
        return pca2129_rtc_epoch_time;

def pca2129_binToBCD(binaryInput):
    shift = int(0)
    bcdResult = int(0)

    while (binaryInput > 0):
        bcdResult |= int(binaryInput % 10) << (shift << 2)
        shift = shift+1
        binaryInput /= 10

    return bcdResult


def pca2129_convertBCD(x):
    return x - 6 * (x >> 4)


def test_alarm_time_1():
    true_date = dt.datetime(2000, 1, 1, 0, 0, 0)
    test_date = TimeTester(2000, 1, 1, 0, 0, 0)

    date = test_date.get_future_time(37000)
    new_test_date = dt.datetime(date['year'], date['month'], date['day'], date['hour'],
                                date['minute'], date['second'])
    assert new_test_date == true_date + timedelta(seconds=37000)

def test_alarm_time_2():
    true_date = dt.datetime(2000, 1, 1, 0, 0, 0)
    test_date = TimeTester(2000, 1, 1, 0, 0, 0)

    date = test_date.get_future_time(2419202)
    new_test_date = dt.datetime(date['year'], date['month'], date['day'], date['hour'],
                                date['minute'], date['second'])
    assert new_test_date == true_date + timedelta(seconds=2419202)

def test_alarm_time_3():
    true_date = dt.datetime(2000, 1, 1, 0, 0, 0)
    test_date = TimeTester(2000, 1, 1, 0, 0, 0)

    date = test_date.get_future_time(2499202)
    new_test_date = dt.datetime(date['year'], date['month'], date['day'], date['hour'],
                                date['minute'], date['second'])
    assert new_test_date == true_date + timedelta(seconds=2499202)

def test_alarm_time_4():
    true_date = dt.datetime(2018, 2, 24, 9, 0, 0)
    test_date = TimeTester(2018, 2, 24, 9, 0, 0)

    date = test_date.get_future_time(2499202)
    new_test_date = dt.datetime(date['year'], date['month'], date['day'], date['hour'],
                                date['minute'], date['second'])
    print('tested date:' + str(new_test_date))
    print('actual date:' + str(true_date + timedelta(seconds=2499202)))
    assert new_test_date == true_date + timedelta(seconds=2499202)


def test_alarm_time_5():
    true_date = dt.datetime(2016, 2, 29, 9, 7, 56)
    test_date = TimeTester(2016, 2, 29, 9, 7, 56)

    date = test_date.get_future_time(34992028)
    new_test_date = dt.datetime(date['year'], date['month'], date['day'], date['hour'],
                                date['minute'], date['second'])
    print('tested date:' + str(new_test_date))
    print('actual date:' + str(true_date + timedelta(seconds=34992028)))

    assert new_test_date == true_date + timedelta(seconds=34992028)

def test_time_6():
    true_date = dt.datetime(2004, 9, 20, 15, 59, 59)
    test_date = TimeTester(2004, 9, 20, 15, 59, 59)

    date = test_date.get_future_time(54997858)
    new_test_date = dt.datetime(date['year'], date['month'], date['day'], date['hour'],
                                date['minute'], date['second'])
    print('tested date:' + str(new_test_date))
    print('actual date:' + str(true_date + timedelta(seconds=54997858)))

    assert new_test_date == true_date + timedelta(seconds=54997858)

def test_time_7():
    true_date = dt.datetime(2038, 6, 25, 23, 0, 45)
    test_date = TimeTester(2038, 6, 25, 23, 0, 45)

    date = test_date.get_future_time(8099980)
    new_test_date = dt.datetime(date['year'], date['month'], date['day'], date['hour'],
                                date['minute'], date['second'])
    print('tested date:' + str(new_test_date))
    print('actual date:' + str(true_date + timedelta(seconds=8099980)))

    assert new_test_date == true_date + timedelta(seconds=8099980)


def test_time_8():
    true_date = dt.datetime(2025, 12, 31, 23, 59, 59)
    test_date = TimeTester(2025, 12, 31, 23, 59, 59)

    date = test_date.get_future_time(258387)
    new_test_date = dt.datetime(date['year'], date['month'], date['day'], date['hour'],
                                date['minute'], date['second'])
    print('tested date:' + str(new_test_date))
    print('actual date:' + str(true_date + timedelta(seconds=258387)))

    assert new_test_date == true_date + timedelta(seconds=258387)

def test_bcd_bin_conversion():
    bcd_list = [0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 
                32, 33, 34, 35, 36, 37, 38, 39, 40, 41, 48, 49, 50, 51, 52, 53, 54, 55, 56, 57,
                64, 65, 66, 67, 68, 69, 70, 71, 72, 73, 80, 81, 82, 83, 84, 85, 86, 87, 88, 89,
                96, 97, 98, 99, 100, 101, 102, 103, 104, 105, 112, 113, 114, 115, 116, 117, 118, 119, 120, 121]

    convertedBinList = [pca2129_binToBCD(i) for i in range(len(bcd_list))]

    print(convertedBinList)
    assert convertedBinList == bcd_list

    bin_list = [i for i in range(len(bcd_list))]
    bin_to_bcd = [pca2129_convertBCD(element) for element in bcd_list]

    assert bin_list == bin_to_bcd

def test_date_to_epoch_1():
    base_date = dt.datetime(2000, 1, 1, 0, 0, 0)
    current_date = dt.datetime(2000, 1, 1, 1, 1, 1)
    test_date = TimeTester(0, 1, 1, 1, 1, 1)

    secondsDelta = (current_date - base_date).total_seconds()

    assert secondsDelta == test_date.pca2129_getCurrentRTCTime()

def test_date_to_epoch_2():
    base_date = dt.datetime(2000, 1, 1, 0, 0, 0)
    current_date = dt.datetime(2000, 1, 1, 0, 0, 0)
    test_date = TimeTester(0, 1, 1, 0, 0, 0)

    secondsDelta = (current_date - base_date).total_seconds()

    assert secondsDelta == test_date.pca2129_getCurrentRTCTime()

def test_date_to_epoch_3():
    base_date = dt.datetime(2000, 1, 1, 0, 0, 0)
    current_date = dt.datetime(2000, 1, 15, 20, 56, 59)
    test_date = TimeTester(0,  1, 15, 20, 56, 59)

    secondsDelta = (current_date - base_date).total_seconds()

    assert secondsDelta == test_date.pca2129_getCurrentRTCTime()

def test_date_to_epoch_3():
    base_date = dt.datetime(2000, 1, 1, 0, 0, 0)
    current_date = dt.datetime(2001, 6, 15, 23, 45, 15)
    test_date = TimeTester(1, 6, 15, 23, 45, 15)

    secondsDelta = (current_date - base_date).total_seconds()

    assert secondsDelta == test_date.pca2129_getCurrentRTCTime()


