#include "SfzParser.h"
#include <algorithm>
#include <cctype>
#include <sstream>

namespace ppgu {

namespace {
int clampI(int v, int lo, int hi) { return std::max(lo, std::min(hi, v)); }

std::string trim(const std::string& s) {
    size_t a = 0, b = s.size();
    while (a < b && std::isspace((unsigned char)s[a])) ++a;
    while (b > a && std::isspace((unsigned char)s[b-1])) --b;
    return s.substr(a, b - a);
}
} // namespace

SfzDocument SfzParser::parse(const std::string& text) {
    SfzDocument doc;
    std::istringstream in(text);
    std::string line;
    SfzRegion cur;
    bool inRegion = false;
    while (std::getline(in, line)) {
        auto hash = line.find('//');
        if (hash != std::string::npos) line = line.substr(0, hash);
        line = trim(line);
        if (line.empty()) continue;
        if (line == "<region>") {
            if (inRegion) doc.regions.push_back(cur);
            cur = SfzRegion{}; inRegion = true;
            continue;
        }
        if (line.rfind("<", 0) == 0) continue; // other opcodes ignored
        auto eq = line.find('=');
        if (eq == std::string::npos) continue;
        std::string k = trim(line.substr(0, eq));
        std::string v = trim(line.substr(eq + 1));
        auto toInt = [](const std::string& s, int def) {
            try { return std::stoi(s); } catch (...) { return def; }
        };
        if (k == "key") { cur.key = clampI(toInt(v, -1), 0, 127); cur.lokey = cur.key; cur.hikey = cur.key; }
        else if (k == "lokey") cur.lokey = clampI(toInt(v, 0), 0, 127);
        else if (k == "hikey") cur.hikey = clampI(toInt(v, 127), 0, 127);
        else if (k == "lovel") cur.lovel = clampI(toInt(v, 0), 0, 127);
        else if (k == "hivel") cur.hivel = clampI(toInt(v, 127), 0, 127);
        else if (k == "sample") cur.sample = v;
        else if (k == "loop_mode") cur.loopMode = (v == "loop" || v == "forward_loop");
        else if (k == "oneshot") cur.oneShot = (v == "on");
        else if (k == "transpose") cur.transpose = clampI(toInt(v, 0), -48, 48);
        else if (k == "seq_length") cur.seqLength = clampI(toInt(v, 1), 1, 16);
        else if (k == "seq_position") cur.seqPosition = clampI(toInt(v, 1), 1, cur.seqLength);
    }
    if (inRegion) doc.regions.push_back(cur);
    for (auto& r : doc.regions) {
        if (r.hikey < r.lokey) std::swap(r.lokey, r.hikey);
        if (r.hivel < r.lovel) std::swap(r.lovel, r.hivel);
    }
    doc.ok = !doc.regions.empty();
    if (!doc.ok) doc.error = "no regions";
    return doc;
}

} // namespace ppgu