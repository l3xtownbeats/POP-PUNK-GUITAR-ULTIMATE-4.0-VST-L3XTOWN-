// PPGU OverlayManager contract tests — O1/O2. Exactly 12 assertions.
#include "OverlayManager.h"
#include <cassert>
#include <string>

using ppgu::OverlayManager;

int runOverlayManagerTests()
{
    // 1. duplicate id rejected
    {
        OverlayManager m;
        OverlayManager::Entry a; a.id = "panel-a"; a.visible = true;
        OverlayManager::Entry b; b.id = "panel-a"; b.visible = true;
        m.push(a);
        assert(m.push(b) == false);
    }
    // 2. capacity hard limit 8; 9th rejected
    {
        OverlayManager m;
        for (int i = 0; i < 8; ++i)
        {
            OverlayManager::Entry e; e.id = "ov" + std::to_string(i); e.visible = true;
            m.push(e);
        }
        std::string t;
        assert(m.topId(t) == true && t == "ov7");
        OverlayManager::Entry e9; e9.id = "ov8"; e9.visible = true;
        assert(m.push(e9) == false);
    }
    // 3. topmost visible modal blocks new windows (O2)
    {
        OverlayManager m;
        OverlayManager::Entry bg; bg.id = "bg"; bg.visible = true;
        OverlayManager::Entry mdl; mdl.id = "modal"; mdl.visible = true; mdl.modal = true;
        m.push(bg);
        assert(m.push(mdl) == true);
        assert(m.blockedForNew() == true);
    }
    // 4. no visible modal — not blocked
    {
        OverlayManager m;
        OverlayManager::Entry bg; bg.id = "bg"; bg.visible = true;
        m.push(bg);
        assert(m.blockedForNew() == false);
    }
    // 5. closeTop honours closable (deterministic refusal)
    {
        OverlayManager m;
        OverlayManager::Entry sys; sys.id = "system"; sys.visible = true; sys.closable = false;
        m.push(sys);
        assert(m.close("system") == false);
    }
    // 6. close removes entry; top falls through to next visible
    {
        OverlayManager m;
        OverlayManager::Entry a; a.id = "panel-a"; a.visible = true;
        OverlayManager::Entry b; b.id = "panel-b"; b.visible = true;
        m.push(a); m.push(b);
        assert(m.close("panel-b") == true);
        std::string t;
        assert(m.topId(t) == true && t == "panel-a");
    }
    // 7. hidden entries are skipped by topId
    {
        OverlayManager m;
        OverlayManager::Entry h; h.id = "hidden"; h.visible = false;
        OverlayManager::Entry v; v.id = "visible"; v.visible = true;
        m.push(h); m.push(v);
        std::string t;
        assert(m.topId(t) == true && t == "visible");
    }
    // 8. modal flag change re-evaluates blocking
    {
        OverlayManager m;
        OverlayManager::Entry mdl; mdl.id = "modal"; mdl.visible = true; mdl.modal = true;
        m.push(mdl);
        assert(m.setModal("modal", false) == true);
        assert(m.blockedForNew() == false);
    }
    return 0;
}
#ifdef PPGU_STANDALONE_TEST
int main() { return runOverlayManagerTests(); }
#endif
