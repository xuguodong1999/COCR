package io.github.xuguodong1999;

import static org.junit.Assert.assertEquals;

import org.junit.Assert;
import org.junit.Test;

import io.reactivex.Single;

/**
 * Example local unit test, which will execute on the development machine (host).
 *
 * @see <a href="http://d.android.com/tools/testing">Testing documentation</a>
 */
public class ExampleUnitTest {
    @Test
    public void addition_isCorrect() {
        assertEquals(4, 2 + 2);
    }


    interface Getter<T> {
        T get();
    }

    <T> T rxSyncWrapper(Getter<T> getter, T errorCode) {
        StackTraceElement[] ste = (new Throwable()).getStackTrace();
        Single<T> single = Single.create(emitter -> {
            try {
                emitter.onSuccess(getter.get());
            } catch (Exception e) {
                emitter.onError(e);
            }
        });
        return single.doOnError(e -> {
            //
            System.out.println(e);
        }).doOnSuccess(v -> {
            //
            System.out.println(v);
        }).doOnTerminate(() -> {
            for (StackTraceElement s : ste) {
                System.out.println(s.getMethodName());
            }
        }).onErrorReturnItem(errorCode).blockingGet();
    }

    int doInternal(boolean cond) {
        if (cond) {
            return 1234;
        }
        throw new RuntimeException("error message");
    }

    int doExternal(boolean cond) {
        return rxSyncWrapper(() -> doInternal(cond), -1);
    }

    @Test
    public void testRxSyncOk() {
        Assert.assertEquals(doExternal(true), 1234);
    }

    @Test
    public void testRxSyncFailure() {
        Assert.assertEquals(doExternal(false), -1);
    }
}