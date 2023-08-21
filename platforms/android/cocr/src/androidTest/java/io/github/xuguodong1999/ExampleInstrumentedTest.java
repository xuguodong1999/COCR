package io.github.xuguodong1999;

import static org.junit.Assert.assertEquals;

import android.content.Context;
import android.os.SystemClock;
import android.util.Log;

import androidx.test.annotation.UiThreadTest;
import androidx.test.ext.junit.runners.AndroidJUnit4;
import androidx.test.platform.app.InstrumentationRegistry;

import org.junit.After;
import org.junit.Test;
import org.junit.runner.RunWith;

import java.util.concurrent.CountDownLatch;
import java.util.concurrent.ExecutorService;
import java.util.concurrent.Executors;
import java.util.concurrent.TimeUnit;

import io.reactivex.Observable;
import io.reactivex.android.schedulers.AndroidSchedulers;
import io.reactivex.disposables.Disposable;
import io.reactivex.plugins.RxJavaPlugins;
import io.reactivex.schedulers.Schedulers;

/**
 * Instrumented test, which will execute on an Android device.
 *
 * @see <a href="http://d.android.com/tools/testing">Testing documentation</a>
 */
@RunWith(AndroidJUnit4.class)
public class ExampleInstrumentedTest {
    private final CountDownLatch mCountDownLatch = new CountDownLatch(1);
    private final int INTERVAL = 200;

    @After
    public void waitAWhile() {
        try {
            Thread.sleep(1000);
        } catch (InterruptedException ignored) {
        }
    }

    @UiThreadTest
    public void useAppContext() {
        // Context of the app under test.
        Context appContext = InstrumentationRegistry.getInstrumentation().getTargetContext();
        assertEquals("io.github.xuguodong1999.example", appContext.getPackageName());
    }

    Observable<Integer> getObForRxFlatMap(final int a) {
        final String TAG = "rxFlatMapDemo";
        return Observable.create(emitter -> {
            Log.d(TAG, "produce thread: " + Thread.currentThread().getId());
            emitter.onNext(a);
        });
    }

    public static final ExecutorService fixedThreadPool = Executors.newFixedThreadPool(4);

    @UiThreadTest
    @Test
    public void rxFlatMapDemo() {
        final String TAG = "rxFlatMapDemo";
        RxJavaPlugins.setErrorHandler(e -> {
            Log.d(TAG, "handler: exception=" + e);
        });
        Log.d(TAG, "test thread: " + Thread.currentThread().getId());
        Observable<Integer> ob1 = getObForRxFlatMap(1);
        // work as getObservable wrapper
        Observable<Integer> ob2 = ob1.map(integer -> {
                    Log.d(TAG, "ob1 map thread: " + Thread.currentThread().getId());
                    return integer + 1;
                })
                .observeOn(AndroidSchedulers.mainThread())
                .subscribeOn(Schedulers.from(fixedThreadPool));
        Observable<Integer> ob4 = ob2.flatMap(integer -> {
            Log.d(TAG, "ob4 thread: " + Thread.currentThread().getId());
            Observable<Integer> ob3 = getObForRxFlatMap(integer + 1000);
            // !!! must subscribe on a specific thread here, or block ui thread
            return ob3.subscribeOn(Schedulers.from(fixedThreadPool));
        });
        // work as getObservable wrapper
        Observable<Integer> ob5 = ob4.map(integer -> {
                    Log.d(TAG, "ob4 map thread: " + Thread.currentThread().getId());
                    return integer + 2;
                })
                .observeOn(AndroidSchedulers.mainThread())
                .subscribeOn(Schedulers.from(fixedThreadPool));
        Disposable timer = ob5.subscribe(integer -> {
            Log.d(TAG, "ob5 consume thread: " + Thread.currentThread().getId());
        }, throwable -> {
            Log.d(TAG, "ob5 die thread: " + Thread.currentThread().getId());
        });
    }

    @UiThreadTest
    @Test
    public void rxFlatMapDemo2() {
        final String TAG = "rxFlatMapDemo";
        RxJavaPlugins.setErrorHandler(e -> {
            Log.d(TAG, "handler: exception=" + e);
        });
        Log.d(TAG, "test thread: " + Thread.currentThread().getId());
        Observable<Integer> ob1 = getObForRxFlatMap(1);
        // work as getObservable wrapper
        Observable<Integer> ob2 = ob1.map(integer -> {
                    Log.d(TAG, "ob1 map thread: " + Thread.currentThread().getId());
                    return integer + 1;
                })
                .observeOn(AndroidSchedulers.mainThread())
                .subscribeOn(Schedulers.from(fixedThreadPool));
        Observable<Integer> ob4 = ob2.flatMap(integer -> {
            Log.d(TAG, "ob4 thread: " + Thread.currentThread().getId());
            Observable<Integer> ob3 = getObForRxFlatMap(integer + 1000);
            // work as getObservable wrapper
            return ob3.map(integer2 -> {
                        Log.d(TAG, "ob3 map thread: " + Thread.currentThread().getId());
                        return integer2 + 2;
                    })
                    .observeOn(AndroidSchedulers.mainThread())
                    .subscribeOn(Schedulers.from(fixedThreadPool));
        });
        Disposable timer = ob4.subscribe(integer -> {
            Log.d(TAG, "ob5 consume thread: " + Thread.currentThread().getId());
        }, throwable -> {
            Log.d(TAG, "ob5 die thread: " + Thread.currentThread().getId());
        });
    }

    @Test
    public void rxCancelDemo() throws InterruptedException {
        final String TAG = "rxCancelDemo";
        RxJavaPlugins.setErrorHandler(e -> {
            Log.d(TAG, "handler: exception=" + e);
        });
        Log.d(TAG, "");
        Disposable d = Observable.create(emitter -> {
                    while (true) {
                        Thread.sleep(INTERVAL);
                        long value = SystemClock.uptimeMillis();
                        Log.d(TAG, "Observable.create: produce value=" + value);
                        emitter.onNext(value);
                    }
                })
                .flatMap(oldValue -> Observable.create(emitter -> {
                    emitter.onNext(oldValue);
                }))
                .subscribeOn(Schedulers.io())
                .observeOn(AndroidSchedulers.mainThread())
                .subscribe(value -> {
                    Log.d(TAG, "Observable.subscribe: value=" + value);
                }, e -> {
                    Log.d(TAG, "Observable.subscribe: exception=" + e.getLocalizedMessage());
                });
        Disposable timer = Observable.interval(
                        INTERVAL * 3, INTERVAL * 3, TimeUnit.MILLISECONDS)
                .take(2)
                .subscribeOn(Schedulers.io())
                .observeOn(AndroidSchedulers.mainThread())
                .subscribe(value -> {
                    Log.d(TAG, "timer: value=" + value);
                    if (!d.isDisposed()) {
                        Log.d(TAG, "dispose");
                        d.dispose();
                    } else {
                        Log.d(TAG, "already disposed");
                        mCountDownLatch.countDown();
                    }
                }, e -> {
                    Log.d(TAG, "exception=" + e.getLocalizedMessage());
                });
        mCountDownLatch.await();
        timer.dispose();
    }
}