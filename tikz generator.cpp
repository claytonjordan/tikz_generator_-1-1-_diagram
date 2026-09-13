// Clayton Jordan August 2026 //

#include <iostream>
#include <vector>
#include <algorithm>
#include <unordered_set>

struct disk {
    int u, v, nw, nz, sd; // side is 1 if on right side of the meridian (gap is on left side of fundamental region), -1 otherwise
};
class tikzgen {
    private:
        int p, a, b, r;
        int paired(int u1) { // returns the pair of an intersection point
        if (u1 <= p) {
            return u1 + p;
        }
        return u1 - p;
    }
        bool flag() { // check conditions from prop 3.4
        if (!(p % 2) || a > (p / 2) || a < 1 || r > p || r < 1 || b < 0 || b >(p - 2 * a)) {
            return true;
        }
        return false;
    }
        void getlines(std::pair<int, int>& wdisk, std::pair<int, int>& zdisk, std::vector<std::pair<int, int>>& base, std::vector<int>& lines, std::vector<disk>& disks) { // store lines in the base diagram) { // I think Doyle only considers the diagrams where the first line below the concentric disks on the left goes to above the concentric disks on the right, 
        //  ie there can't be upwards arcs on the left which don't connect above the right disks. ask Mike. if not idk how to construct the lines
        int idx = 1;
        for (int i = 1; i <= a; i++) { // left disks
            base[idx] = { i, 2 * a - i + 1 };
            if (2 * a - 2 * i + 1 == 1) {
                zdisk = { i, i + 1 };
            }
            disks.push_back({ i, 2 * a - i + 1, 0 , 1, 1 }); // left disks have nw = 0, nz = 1
            idx++;
        }
        int cnt = 1, rot = r;
        if (b == 0) { // handle edge case
            cnt += 2 * a;
            rot = (rot - 2 * a + p) % p;
        }
        for (int i = 2 * a + 1; i <= 2 * a + b; i++) { // below to above
            base[idx] = { i, p + cnt };
            idx++;
            cnt++;
        }
        int top = p + b + 1;
        int bot = top + 2 * a - 1;
        for (int i = 0; i < a; i++) { // handle right disks;
            base[idx] = { top + i, bot - i };
            idx++;
        }
        for (int cnt = 1, i = 2 * a + b + 1; i <= p; i++) { // handle below to below
            base[idx] = { i, bot + cnt };
            idx++;
            cnt++;
        }
        for (int i = a + 1; i <= p; i++) { // apply rotation to every line after the left concentric disks
            std::pair<int, int> cur = base[i];
            if (cur.second > p && cur.first > p) { // right concentric disk
                if (cur.first + rot - 1 > 2 * p) {
                    cur.first -= p;
                }
                if (cur.second + rot - 1 > 2 * p) {
                    cur.second -= p;
                }
                int top = cur.first + rot - 1, bot = cur.second + rot - 1;
                base[i] = { top,  bot };
                if (bot - top == 1 || (top == 2 * p && bot == p + 1)) {
                    wdisk = { top, bot };
                }
                disks.push_back({ top, bot, 1, 0, -1 }); // right disks have nw = 1, nz = 0
            }
            else { // not a right concentric disk
                if (cur.second + rot - 1 > 2 * p) {
                    cur.second -= p;
                }
                base[i] = { cur.first, cur.second + rot - 1 };
            }

        }
        for (int i = 1; i <= p; i++) {
            lines[base[i].first] = base[i].second;
            lines[base[i].second] = base[i].first;
        }
    }
        std::pair<int, int> isconnected(const std::vector<int>& lines) {
        // check if alpha is connected. return also the intersection number.
        // we start from the topmost point on the left, then traverse that curve CCW, then proceed in the obvious way from there. 
        // if we pass through the left side from the interior of the fundamental region, increment by 1. if we pass through from the right side boundary, decrement by 1
        // every line we traverse, we will cross the left side exactly one time (obviously)
        // if we are traversing a left side concentric disk, or a line from the right to the left, we increment. if not, we decrement
        // from proposition 3.4 in Doyle
        std::vector<int> visited(2 * (p + 1), 0); // we want to visit every point exactly once. make sure this has size 2 * p
        int cur = 1, cr = 0;
        while (1) {
            if (visited[cur]) {
                return { 0, cr };
            }
            visited[cur] = 1;
            if ((cur <= p && lines[cur] <= p) || (cur > p && lines[cur] <= p)) { // first condition: left concentric disk. second condition: line from R to L
                cr++;
            }
            else {
                cr--;
            }
            cur = lines[cur];
            if (visited[cur]) {
                return { 0, cr };
            }
            visited[cur] = 1;
            cur = paired(cur);
            if (cur == 1) {
                break;
            }
        }
        for (int i = 1; i <= 2 * p; i++) {
            if (!visited[i]) {
                return { 0, cr };
            }
        }
        return { 1, cr };
    }
        std::pair<int, int> getdisk(int start, int end, const std::vector<disk>& disks) {
        for (int i = 0; i < disks.size(); i++) {
            int U = disks[i].u, V = disks[i].v;
            if ((U == start && V == end) || (U == end && V == start)) {
                return { U, V };
            }
        }
        return { 0,0 };
    }
        std::pair<int, int> crosses(int start, int end, const std::vector<disk>& disks, const std::vector<int>& lines) {
        if (start > p) { // we are on the right side
            if (end > p) { // check for disks which pass through the boundary
                std::pair<int, int> d = getdisk(start, end, disks);
                if (d.first > d.second) { // this disk does cross the boundary
                    if (start > end) { // crossing upwards
                        return { 1, 1 };
                    }
                    return { 1, -1 }; // crossing downwards
                }
                return { 0, 0 };
            }
            for (int i = start + 1; i <= 2 * p; i++) {
                if (lines[i] < end) {
                    return { 1, 1 };
                }
            }

            return { 0, 0 };
        }
        else { // we are on the left side
            if (end < p) { // note that left disks will never pass through the boundary
                return { 0, 0 };
            }
            for (int i = start - 1; i >= 1; i--) {
                if (lines[i] > end) {
                    return { 1, -1 };
                }
            }
            return { 0, 0 };
        }
    }
        int verticalcrossing(const std::vector<disk>& disks, const std::vector<int>& lines) { // same as isconnected but we want to check the vertical intersection number instead of horizontal. if its 0, we can directly get the gauss code. if not, we need to dehn twist
        std::vector<int> visited(2 * (p + 1), 0); // we want to visit every point exactly once
        int cur = 1, cr = 0;
        while (1) { // since we know its connected, we can modify the procedure a little
            if (visited[cur]) {
                return cr;
            }
            visited[cur] = 1;
            std::pair<int, int> cross = crosses(cur, lines[cur], disks, lines);
            if (cross.first == 1) {
                cr += cross.second;
            }
            cur = lines[cur];
            visited[cur] = 1;
            cur = paired(cur);
        }
        return 999999;
    }
        void generatetikz(const std::pair<int, int>& wdisk, const std::pair<int, int>& zdisk, const std::vector<int>& lines, const std::vector<disk>& disks) {
            std::cout << std::endl;
            std::cout << std::endl;
            std::cout << "\\draw[thick, black] (0,0) -- (" << p + 1 << ", 0);" << std::endl;
            std::cout << "\\draw[thick, red] (" << p + 1 << ", 0) -- (" << p + 1 << ", " << p + 1 << ");" << std::endl;
            std::cout << "\\draw[thick, black] (" << p + 1 << ", " << p + 1 << ") -- (0, " << p + 1 << ");" << std::endl;
            std::cout << "\\draw[thick, red] (0, " << p + 1 << ") -- (0, 0);" << std::endl;
            std::cout << std::endl;
            int crn = 0;
            double midl = (double)(zdisk.second + zdisk.first) / 2;
            double midr = (double)(wdisk.second + wdisk.first) / 2 - p;
            for (int i = 0; i < a; i++) { // left disks
                disk d = disks[i];
                int u = d.u, v = d.v;
                u = p + 1 - u;
                v = p + 1 - v;
                int sz = a - i;
                std::cout << "\\draw[blue, thick] plot[smooth, tension=1.5]" << std::endl;
                std::cout << "coordinates {(0," << u << ") (" << sz << "," << p + 1 - midl << ") (0," << v << ")};" << std::endl;
            }
            std::cout << std::endl;
            std::vector<std::pair<int, int>> crossed;
            std::vector<std::pair<int, int>> ncross;
            for (int i = 1; i <= p; i++) {
                if (crosses(i, lines[i], disks, lines).first) {
                    crossed.push_back({ i, lines[i] });
                }
                else if (lines[i] > p) { // not a disk
                    ncross.push_back({ i, lines[i] });
                }
            }
            for (int i = a; i < 2 * a; i++) {
                if (disks[i].u > disks[i].v) {
                    crn++;
                }
            }
            int sz = crossed.size() + crn;
            double loc = (double)(p + 1) / (double)(sz + 1);
            for (int i = a + crn; i < 2 * a; i++) { // right disks
                disk d = disks[i];
                int u = paired(d.u), v = paired(d.v);
                u = p + 1 - u;
                v = p + 1 - v;
                int sz = 2 * a - i;
                std::cout << "\\draw[blue, thick] plot[smooth, tension=1.5]" << std::endl;
                std::cout << "coordinates {(" << p + 1 << "," << u << ") (" << p + 1 - sz << "," << p + 1 - midr << ") (" << p + 1 << "," << v << ")};" << std::endl;
            }
            for (int i = a; i < a + crn; i++) { // handle disks which cross through
                disk d = disks[i];
                int u = paired(d.u), v = paired(d.v);
                u = p + 1 - u;
                v = p + 1 - v;
                int sz = 2 * a - i, diff = u + (p + 1 - v);
                double midb = (double)(u + v + p) / 2 - (p + 0.5);
                double midt = p + 0.5 + (double)(u + v - p) / 2;
                std::cout << "\\begin{scope}" << std::endl << "\\clip (0,0) rectangle (" << p + 1 << "," << p + 1 << ");" << std::endl << "\\draw[blue, thick] plot[smooth, tension=1.5]" << std::endl << "coordinates {(";
                std::cout << p + 1 << "," << u << ") (" << p + 1 - sz << "," << midb << ") (" << p + 1 << "," << u - diff << ")};" << std::endl;
                std::cout << "\\end{scope}" << std::endl;
                std::cout << std::endl;
                std::cout << "\\begin{scope}" << std::endl << "\\clip (0,0) rectangle (" << p + 1 << "," << p + 1 << ");" << std::endl << "\\draw[blue, thick] plot[smooth, tension=1.5]" << std::endl << "coordinates {(";
                std::cout << p + 1 << "," << v << ") (" << p + 1 - sz << "," << midt << ") (" << p + 1 << "," << v + diff << ")};" << std::endl;
                std::cout << "\\end{scope}" << std::endl;
                std::cout << std::endl;
            }
            std::cout << std::endl;
            for (int i = 0; i < ncross.size(); i++) {
                int from = ncross[i].first, to = ncross[i].second - p;
                if (to < wdisk.first - p) { // below to above
                    std::cout << "\\draw[blue, thick] (0," << p + 1 - from << ") to[out=2, in=-178] (" << p + 1 << "," << p + 1 - to << ");" << std::endl;
                }
                else {
                    std::cout << "\\draw[blue, thick] (0," << p + 1 - from << ") to[out=-2, in=178] (" << p + 1 << "," << p + 1 - to << ");" << std::endl;
                }
            }
            std::cout << std::endl;
            // there may be issues with arcs intersecting each other. this can be fixed by changing loc (making it smaller), or changing the out and in values of the lines which cross.
            // this gives a good starting point for most diagrams though
            for (int i = 0; i < crossed.size(); i++) {
                int from = crossed[i].first, to = crossed[i].second - p;
                std::cout << "\\draw[blue, thick] (0," << p + 1 - from << ") to[out=-2, in=166] (" << loc * (double)(sz - crn - i) << ",0);" << std::endl;
                std::cout << "\\draw[blue, thick] (" << p + 1 << "," << p + 1 - to << ") to[out=178, in=-14] (" << loc * (double)(sz - crn - i) << "," << p + 1 << ");" << std::endl;
            }
            std::cout << std::endl;

            std::cout << "\\fill[black] (0.5," << p + 1 - midl << ") circle (2.5pt);" << std::endl;
            if (wdisk.first > wdisk.second) { // smallest disk crosses boundary
                std::cout << "\\draw[black] (" << (double)p + 0.5 << "," << p + 0.5 << ") circle (2.5pt);" << std::endl;
            }
            else {
                std::cout << "\\draw[black] (" << (double)p + 0.5 << "," << p + 1 - midr << ") circle (2.5pt);" << std::endl;
            }
            std::cout << std::endl;
        }
    public:
        tikzgen(int p_, int a_, int b_, int r_) : p(p_), a(a_), b(b_), r(r_) {}
        int gen() {
            std::pair<int, int> wdisk, zdisk; // smallest disks containing w and z respectively
            std::vector<std::pair<int, int>> base((p + 1), { 0, 0 }); // store lines in the base diagram. p <= 101
            std::vector<int> lines(2 * (p + 1), 0);
            std::vector<disk> disks;
            if (flag()) {
                std::cout << "This 4-tuple doesn't describe a (1,1)-knot" << std::endl;
                return 1;
            }
            getlines(wdisk, zdisk, base, lines, disks);
            std::pair<int, int> flagg = isconnected(lines);
            int conn = flagg.first, cr = flagg.second;
            if (!conn) { // check alpha is connected
                std::cout << "alpha is not connected and the 4-tuple doesn't describe a (1,1)-knot" << std::endl;
                return 1;
            }
            if (abs(cr) != 1) { // check the condition for the intersection number. see the comments in isconnected() for a definition of the intersection number
                std::cout << "the 4-tuple doesn't describe a (1,1)-knot because the intersection number is not +-1" << std::endl;
                return 1;
            }

            generatetikz(wdisk, zdisk, lines, disks);

            return 0;
        }
};
int main() {
    std::cout << "Input your desired 4-tuple parametrization (p, a, b, r). To exit, input -1" << std::endl;
    int p, a, b, r;
    std::cin >> p;
    while (p != -1) {
        std::cin >> a >> b >> r;
        tikzgen knot(p, a, b, r);
        knot.gen();
        std::cout << std::endl;
        std::cout << "Input your desired 4-tuple parametrization (p, a, b, r). To exit, input -1" << std::endl;
        std::cin >> p;
    }
    return 0;
}