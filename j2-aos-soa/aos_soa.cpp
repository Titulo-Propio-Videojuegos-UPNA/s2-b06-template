// J2 (S2, bloque 2.2): el mismo paso de simulacion con Array of Structs (AoS) y con
// Struct of Arrays (SoA). Solo se usan 4 de los 16 floats de cada entidad: en AoS se
// arrastran a la cache 64 bytes por entidad; en SoA, 16. Uso: ./aos_soa [max_entities]
// Medida: 5 repeticiones alternando el orden (AoS-SoA, SoA-AoS...); se publica la mediana y el rango del cociente.
#include <algorithm>
#include <array>
#include <chrono>
#include <cmath>
#include <cstdio>
#include <cstdlib>
#include <vector>

namespace {

struct EntityAoS {           // 64 bytes: una linea de cache tipica
    float x, y, vx, vy;      // lo que usa el paso de simulacion
    float extra[12];         // salud, color, ids, timers... (no se usan aqui)
};

struct EntitiesSoA {
    std::vector<float> x, y, vx, vy;
    std::vector<float> extra; // 12 por entidad, en otro bloque de memoria
};

void step_aos(std::vector<EntityAoS>& e, float dt) {
    for (auto& p : e) {
        p.x += p.vx * dt;
        p.y += p.vy * dt;
        if (p.x < 0.f || p.x > 1000.f) p.vx = -p.vx;
        if (p.y < 0.f || p.y > 1000.f) p.vy = -p.vy;
    }
}

void step_soa(EntitiesSoA& e, float dt) {
    const std::size_t n = e.x.size();
    for (std::size_t i = 0; i < n; ++i) {
        e.x[i] += e.vx[i] * dt;
        e.y[i] += e.vy[i] * dt;
        if (e.x[i] < 0.f || e.x[i] > 1000.f) e.vx[i] = -e.vx[i];
        if (e.y[i] < 0.f || e.y[i] > 1000.f) e.vy[i] = -e.vy[i];
    }
}

float frand(unsigned& s) { s = s * 1664525u + 1013904223u; return static_cast<float>(s >> 8) / 16777216.0f; }

template <class F>
double ns_per_entity(F&& f, std::size_t n, int reps) {
    f(); // calentamiento
    const auto t0 = std::chrono::steady_clock::now();
    for (int r = 0; r < reps; ++r) f();
    const auto t1 = std::chrono::steady_clock::now();
    return std::chrono::duration<double, std::nano>(t1 - t0).count() / (static_cast<double>(n) * reps);
}

} // namespace

int main(int argc, char** argv) {
    const std::size_t max_n = argc > 1 ? std::strtoul(argv[1], nullptr, 10) : 10'000'000ul;
    const float dt = 1.0f / 120.0f;
    constexpr int kTrials = 5;
    std::printf("%10s %12s %12s %8s %15s %s\n", "entidades", "AoS ns/ent", "SoA ns/ent", "AoS/SoA", "rango", "iguales");
    for (std::size_t n = 10'000; n <= max_n; n *= 10) {
        std::vector<EntityAoS> aos(n);
        EntitiesSoA soa{std::vector<float>(n), std::vector<float>(n), std::vector<float>(n), std::vector<float>(n),
                        std::vector<float>(n * 12)};
        unsigned s = 12345u;
        for (std::size_t i = 0; i < n; ++i) {
            aos[i] = {frand(s) * 1000.f, frand(s) * 1000.f, frand(s) * 200.f - 100.f, frand(s) * 200.f - 100.f, {}};
            soa.x[i] = aos[i].x; soa.y[i] = aos[i].y; soa.vx[i] = aos[i].vx; soa.vy[i] = aos[i].vy;
        }
        const int reps = static_cast<int>(std::max<std::size_t>(1, 100'000'000ul / n));
        std::array<double, kTrials> ta{}, tb{}, ratio{};
        for (int t = 0; t < kTrials; ++t) { // orden alterno: no favorece siempre al que va segundo
            if (t % 2 == 0) { ta[t] = ns_per_entity([&] { step_aos(aos, dt); }, n, reps); tb[t] = ns_per_entity([&] { step_soa(soa, dt); }, n, reps); }
            else            { tb[t] = ns_per_entity([&] { step_soa(soa, dt); }, n, reps); ta[t] = ns_per_entity([&] { step_aos(aos, dt); }, n, reps); }
            ratio[t] = ta[t] / tb[t];
        }
        auto median = [](std::array<double, kTrials> v) { std::sort(v.begin(), v.end()); return v[kTrials / 2]; };
        const auto [rmin, rmax] = std::minmax_element(ratio.begin(), ratio.end());
        bool same = true; // los dos disenos deben dar exactamente el mismo resultado (los cuatro campos)
        for (std::size_t i = 0; i < n && same; ++i)
            same = aos[i].x == soa.x[i] && aos[i].y == soa.y[i] && aos[i].vx == soa.vx[i] && aos[i].vy == soa.vy[i];
        std::printf("%10zu %12.3f %12.3f %8.2f %6.2f - %6.2f %s\n", n, median(ta), median(tb), median(ratio), *rmin, *rmax,
                    same ? "si" : "NO");
    }
}
