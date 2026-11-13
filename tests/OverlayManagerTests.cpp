// O1/O2 contract tests. Exactly 12 checks; no assert/NDEBUG dependency.
#include "OverlayManager.h"

#include <iostream>
#include <string>

namespace
{
int checks = 0;
int failures = 0;
#define CHECK(expression) do { \
    ++checks; \
    if (!(expression)) { \
        ++failures; \
        std::cerr << "FAIL line " << __LINE__ << ": " #expression "\n"; \
    } \
} while (false)

class TestListener final : public ppgu::Listener
{
public:
    explicit TestListener(ppgu::OverlayManager& managerIn) : manager(managerIn) {}

    void overlayClosed(const std::string&, bool byUser) override
    {
        ++closed;
        lastByUser = byUser;
        std::string top;
        reentrantQuery = manager.topId(top);
    }

    void overlayRaised(const std::string&) override
    {
        ++raised;
        std::string top;
        reentrantQuery = manager.topId(top);
    }

    void overlayAdded(const std::string&) override
    {
        ++added;
        std::string top;
        reentrantQuery = manager.topId(top);
    }

    ppgu::OverlayManager& manager;
    int added = 0;
    int closed = 0;
    int raised = 0;
    bool lastByUser = false;
    bool reentrantQuery = false;
};
}

int main()
{
    ppgu::OverlayManager manager;
    TestListener listener(manager);
    manager.setListener(&listener);

    ppgu::OverlayManager::Entry first { "A", false, true, true };
    ppgu::OverlayManager::Entry duplicate { "A", false, true, true };
    CHECK(manager.push(first) && !manager.push(duplicate));

    bool filled = true;
    for (char id = 'B'; id <= 'G'; ++id)
        filled = manager.push({ std::string(1, id), false, true, true }) && filled;
    filled = manager.push({ "H", false, true, false }) && filled;
    std::string top;
    CHECK(filled && manager.size() == ppgu::OverlayManager::kCapacity
          && listener.added == 8 && listener.reentrantQuery
          && manager.topId(top) && top == "H");

    CHECK(!manager.push({ "I", false, true, true }));

    CHECK(manager.setModal("H", true) && manager.blockedForNew()
          && manager.topModalId(top) && top == "H");

    CHECK(manager.setVisible("H", false) && !manager.blockedForNew()
          && manager.topId(top) && top == "G");

    CHECK(manager.setVisible("H", true) && manager.blockedForNew());

    CHECK(!manager.closeTop());

    CHECK(!manager.close("H"));

    CHECK(manager.setVisible("H", false) && manager.closeTop()
          && manager.topId(top) && top == "F");

    CHECK(manager.setModal("H", false) && manager.setVisible("H", true)
          && !manager.blockedForNew() && !manager.topModalId(top));

    CHECK(manager.raise("A") && manager.topId(top) && top == "A"
          && listener.raised == 1 && listener.reentrantQuery);

    CHECK(manager.close("A") && listener.closed == 1
          && listener.lastByUser && listener.reentrantQuery);

    if (failures != 0)
    {
        std::cerr << failures << " OverlayManager check(s) failed out of "
                  << checks << ".\n";
        return 1;
    }
    std::cout << checks << "/" << checks << " PASS\n";
    return checks == 12 ? 0 : 2;
}
