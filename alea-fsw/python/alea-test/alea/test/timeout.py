from threading import Thread
import functools, time

# @see https://stackoverflow.com/questions/21827874/timeout-a-function-windows
def timeout(timeout):
    def deco(func):
        @functools.wraps(func)
        def wrapper(*args, **kwargs):
            res = [Exception('Test [%s] timeout [%s seconds] exceeded!' % (func.__name__, timeout))]

            def newFunc():
                try:
                    res[0] = func(*args, **kwargs)
                except Exception as e:
                    res[0] = e

            t = Thread(target=newFunc)
            t.daemon = True
            ts = time.time()
            try:
                t.start()
                t.join(timeout)
            except Exception as je:
                print('error starting thread')
                raise je
            ret = res[0]
            if isinstance(ret, BaseException):
                raise ret
                #print(ret.__str__())
                #assert False
            te = time.time()
            print('Test [%s] took [%2.2f] ms' % (func.__name__, (te - ts) * 1000))
            return ret

        return wrapper

    return deco
