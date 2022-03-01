#include "rxcpp/rx.hpp"
#include <iostream>
#include <array>
#include <thread>
#include <mutex>

std::mutex console_mutex;

void CTDetails(int val = 0) {
    console_mutex.lock();
    std::cout << "Current Thread id => "
              << std::this_thread::get_id()
              << val
              << std::endl;
    console_mutex.unlock();
}

void Yield(bool y) {
    if (y) { std::this_thread::yield(); }
}

namespace Rx {
    using namespace rxcpp;
    using namespace rxcpp::sources;
    using namespace rxcpp::operators;
    using namespace rxcpp::util;
}
namespace rxu = rxcpp::util;

int main() {

    {
        printf("Usage: subscribe!!!!!!\n");
        auto observable = rxcpp::observable<>::range(1, 12);
        observable.subscribe([](int v) {
            printf("OnNext: %d\n", v);
        }, []() {
            printf("OnCompleted\n");
        });
    }
    {
        printf("Usage: filter-map!!!!!!\n");
        auto values = rxcpp::observable<>::range(1, 12).filter([](int v) {
            return v % 2 == 0;
        }).map([](int x) {
            return x * x;
        });
        values.subscribe([](
                int v
        ) {
            printf("OnNext: %d\n", v);
        }, []() {
            printf("OnCompleted\n");
        });
    }
    {
        printf("Usage: container!!!!!!\n");
        std::array<int, 3> a = {{1, 2, 3}};
        auto values1 = rxcpp::observable<>::iterate(a);
        values1.subscribe([](int v) {
            printf("OnNext: %d\n", v);
        }, []() {
            printf("OnCompleted\n");
        });
    }
    {
        printf("Usage: subscriber!!!!!!\n");
        auto ints = rxcpp::observable<>::create<int>([](rxcpp::subscriber<int> s) {
            s.on_next(1);
            s.on_next(4);
            s.on_next(9);
            s.on_completed();
        });
        ints.subscribe([](int v) {
            printf("OnNext: %d\n", v);
        }, []() {
            printf("OnCompleted\n");
        });
    }
    {
        printf("Usage: concat!!!!!!\n");
        auto o1 = rxcpp::observable<>::range(1, 3);
        auto o2 = rxcpp::observable<>::range(4, 6);
        auto values = o1.concat(o2);
        values.subscribe([](int v) {
            printf("OnNext: %d\n", v);
        }, []() {
            printf("OnCompleted\n");
        });
    }
    {
        printf("Usage: just!!!!!!\n");
        auto values = rxcpp::observable<>::just(1);
        values.subscribe([](int v) {
            printf("OnNext: %d\n", v);
        }, []() {
            printf("OnCompleted\n");
        });
    }
    {
        printf("Usage: take-map-apply2!!!!!!\n");
        // infinite (until overflow) stream of integers
        auto values = rxcpp::observable<>::range(1);

        auto s1 = values.take(3).map([](int prime) {
            return std::make_tuple("1:", prime);
        });

        auto s2 = values.take(3).map([](int prime) {
            return std::make_tuple("2:", prime);
        });
        s1.concat(s2).subscribe(rxcpp::util::apply_to([](const char *s, int p) {
            printf("%s %d\n", s, p);
        }));
        // optional
//        s1.concat(s2).subscribe([](std::tuple<const char *, int> p) {
//            printf("%s, %d\n", std::get<0>(p), std::get<1>(p));
//        });
    }
    {
        printf("Usage: unsubscribe!!!!!!\n");
        auto subs = rxcpp::composite_subscription();
        auto values = rxcpp::observable<>::range(1, 10);
        values.subscribe(subs, [&subs](int v) {
            printf("OnNext: %d\n", v);
            if (v == 6)
                subs.unsubscribe(); //-- Stop recieving events
        }, []() {
            printf("OnCompleted\n");
        });
    }
    {
        printf("Usage: map!!!!!!\n");
        auto ints = rxcpp::observable<>::range(1, 10).map([](int n) {
            return n * n;
        });
        ints.subscribe([](int v) {
            printf("OnNext: %d\n", v);
        }, []() {
            printf("OnCompleted\n");
        });
    }
    {
        printf("Usage: average!!!!!!\n");
        auto values = rxcpp::observable<>::range(1, 20).average();
        values.subscribe([](double v) {
            printf("average: %lf\n", v);
        }, []() {
            printf("OnCompleted\n");
        });
    }
    {
        printf("Usage: scan!!!!!!\n");
        int count = 0;
        auto values = rxcpp::observable<>::range(1, 20).scan(0, [&count](int seed, int v) {
            count++;
            return seed + v;
        });
        values.subscribe([&](int v) {
            printf("Average through Scan: %f\n", (double) v / count);
        }, []() {
            printf("OnCompleted\n");
        });
    }
    {
        printf("Usage: pipeline!!!!!!\n");
        auto ints = rxcpp::observable<>::range(1, 10) |
                    Rx::map([](int n) { return n * n; });
        ints.subscribe([](int v) {
            printf("OnNext: %d\n", v);
        }, []() {
            printf("OnCompleted\n");
        });
    }
    {
        printf("Usage: scheduler!!!!!!\n");
        auto values = rxcpp::observable<>::range(1, 4).map([](int v) {
            return v * v;
        });
        std::cout << "Main Thread id => "
                  << std::this_thread::get_id()
                  << std::endl;
        values.observe_on(rxcpp::synchronize_new_thread()).as_blocking().subscribe([](int v) {
            std::cout << "Observable Thread id => "
                      << std::this_thread::get_id()
                      << "  " << v << std::endl;
        }, []() {
            std::cout << "OnCompleted" << std::endl;
        });
        std::cout << "Main Thread id => "
                  << std::this_thread::get_id()
                  << std::endl;
    }
    {
        printf("Usage: observeOn!!!!!!\n");
        auto coordination = rxcpp::serialize_new_thread();
        auto worker = coordination.create_coordinator().get_worker();
        auto values = rxcpp::observable<>::interval(
                std::chrono::milliseconds(50)).take(5).replay(coordination);
        worker.schedule([&](const rxcpp::schedulers::schedulable &) {
            values.subscribe([](long v) {
                CTDetails(v);
            }, []() {
                CTDetails();
            });
        });
        worker.schedule(coordination.now()
                        + std::chrono::milliseconds(125), [&](const rxcpp::schedulers::schedulable &) {
            values.subscribe([](long v) {
                CTDetails(v * v);
            }, []() {
                CTDetails();
            });
        });
        worker.schedule([&](const rxcpp::schedulers::schedulable &) {
            values.connect();
        });
        values.as_blocking().subscribe();
    }
    {
        printf("Usage: flatMap!!!!!!\n");
        std::array<std::string, 4> a = {{"Praseed", "Peter", "Sanjay", "Raju"}};
        auto values = rxcpp::observable<>::iterate(a).flat_map([](std::string v) {
            std::array<std::string, 3> salutation = {{"Mr.", "Monsieur", "Sri"}};
            return rxcpp::observable<>::iterate(salutation);
        }, [](std::string f, std::string s) {
            return s + " " + f;
        });
        values.subscribe([](std::string f) {
            std::cout << f << std::endl;
        }, []() {
            std::cout << "Hello World.." << std::endl;
        });
    }
    return 0;
    {
        printf("Usage: !!!!!!\n");
    }
    {
        printf("Usage: !!!!!!\n");
    }
    {
        printf("Usage: !!!!!!\n");
    }
    {
        printf("Usage: !!!!!!\n");
    }
}