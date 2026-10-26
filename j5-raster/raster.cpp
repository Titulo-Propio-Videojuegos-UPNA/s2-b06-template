// J5 (S2, bloque 2.5): rasterizador por software minimo.
// Matriz de modelo-vista-proyeccion, division por w, viewport, rasterizacion por funciones de arista
// con regla top-left, interpolacion de color y z-buffer. Escribe una imagen PPM y su hash (FNV-1a).
// Uso: ./raster [salida.ppm] [angulo_grados]
#include <algorithm>
#include <array>
#include <cmath>
#include <cstdlib>
#include <cstdint>
#include <cstdio>
#include <vector>

namespace r {

struct V3 { float x, y, z; };
struct V4 { float x, y, z, w; };
using M4 = std::array<float, 16>; // fila mayor

M4 mul(const M4& a, const M4& b) {
    M4 c{};
    for (int i = 0; i < 4; ++i)
        for (int j = 0; j < 4; ++j)
            for (int k = 0; k < 4; ++k) c[static_cast<std::size_t>(i * 4 + j)] += a[static_cast<std::size_t>(i * 4 + k)] * b[static_cast<std::size_t>(k * 4 + j)];
    return c;
}
V4 mul(const M4& m, V3 v) {
    return {m[0] * v.x + m[1] * v.y + m[2] * v.z + m[3], m[4] * v.x + m[5] * v.y + m[6] * v.z + m[7],
            m[8] * v.x + m[9] * v.y + m[10] * v.z + m[11], m[12] * v.x + m[13] * v.y + m[14] * v.z + m[15]};
}
M4 rot_y(float a) { const float c = std::cos(a), s = std::sin(a); return {c, 0, s, 0, 0, 1, 0, 0, -s, 0, c, 0, 0, 0, 0, 1}; }
M4 translate(float x, float y, float z) { return {1, 0, 0, x, 0, 1, 0, y, 0, 0, 1, z, 0, 0, 0, 1}; }
M4 perspective(float fovy, float aspect, float n, float f) {
    const float t = 1.0f / std::tan(fovy / 2);
    return {t / aspect, 0, 0, 0, 0, t, 0, 0, 0, 0, (f + n) / (n - f), 2 * f * n / (n - f), 0, 0, -1, 0};
}

struct Image {
    int w, h;
    std::vector<std::uint8_t> rgb;
    std::vector<float> depth;
    Image(int w_, int h_) : w(w_), h(h_), rgb(static_cast<std::size_t>(w_ * h_ * 3), 20), depth(static_cast<std::size_t>(w_ * h_), 1e30f) {}
};

float edge(float ax, float ay, float bx, float by, float px, float py) { return (bx - ax) * (py - ay) - (by - ay) * (px - ax); }
bool top_left(float ax, float ay, float bx, float by) { return (ay == by && bx < ax) || (by > ay); }

// Triangulo en espacio de pantalla (x,y en pixeles, z en [0,1]) con color por vertice.
void draw(Image& img, const std::array<V3, 3>& p, const std::array<V3, 3>& col) {
    const float area = edge(p[0].x, p[0].y, p[1].x, p[1].y, p[2].x, p[2].y);
    if (area >= 0) return; // culling de caras traseras (orden horario en pantalla)
    const int x0 = std::max(0, static_cast<int>(std::floor(std::min({p[0].x, p[1].x, p[2].x}))));
    const int x1 = std::min(img.w - 1, static_cast<int>(std::ceil(std::max({p[0].x, p[1].x, p[2].x}))));
    const int y0 = std::max(0, static_cast<int>(std::floor(std::min({p[0].y, p[1].y, p[2].y}))));
    const int y1 = std::min(img.h - 1, static_cast<int>(std::ceil(std::max({p[0].y, p[1].y, p[2].y}))));
    for (int y = y0; y <= y1; ++y)
        for (int x = x0; x <= x1; ++x) {
            const float px = static_cast<float>(x) + 0.5f, py = static_cast<float>(y) + 0.5f;
            float w0 = edge(p[1].x, p[1].y, p[2].x, p[2].y, px, py);
            float w1 = edge(p[2].x, p[2].y, p[0].x, p[0].y, px, py);
            float w2 = edge(p[0].x, p[0].y, p[1].x, p[1].y, px, py);
            // regla top-left (con area negativa, "dentro" es <= 0)
            if (w0 > 0 || w1 > 0 || w2 > 0) continue;
            if ((w0 == 0 && !top_left(p[2].x, p[2].y, p[1].x, p[1].y)) || (w1 == 0 && !top_left(p[0].x, p[0].y, p[2].x, p[2].y)) ||
                (w2 == 0 && !top_left(p[1].x, p[1].y, p[0].x, p[0].y)))
                continue;
            w0 /= area; w1 /= area; w2 /= area;
            const float z = w0 * p[0].z + w1 * p[1].z + w2 * p[2].z;
            const auto i = static_cast<std::size_t>(y * img.w + x);
            if (z >= img.depth[i]) continue;
            img.depth[i] = z;
            img.rgb[i * 3 + 0] = static_cast<std::uint8_t>(255 * (w0 * col[0].x + w1 * col[1].x + w2 * col[2].x));
            img.rgb[i * 3 + 1] = static_cast<std::uint8_t>(255 * (w0 * col[0].y + w1 * col[1].y + w2 * col[2].y));
            img.rgb[i * 3 + 2] = static_cast<std::uint8_t>(255 * (w0 * col[0].z + w1 * col[1].z + w2 * col[2].z));
        }
}

std::uint64_t fnv1a(const std::vector<std::uint8_t>& d) {
    std::uint64_t h = 1469598103934665603ull;
    for (auto b : d) { h ^= b; h *= 1099511628211ull; }
    return h;
}

} // namespace r

int main(int argc, char** argv) {
    const char* out = argc > 1 ? argv[1] : "cube.ppm";
    const float ang = (argc > 2 ? static_cast<float>(std::atof(argv[2])) : 30.0f) * 3.14159265f / 180.0f;
    const int W = 320, H = 240;
    r::Image img(W, H);
    const r::M4 mvp = r::mul(r::perspective(1.0f, float(W) / H, 0.1f, 100.0f), r::mul(r::translate(0, 0, -4), r::rot_y(ang)));
    // Cubo: 8 vertices, 12 triangulos, color = posicion normalizada
    const std::array<r::V3, 8> v{{{-1, -1, -1}, {1, -1, -1}, {1, 1, -1}, {-1, 1, -1}, {-1, -1, 1}, {1, -1, 1}, {1, 1, 1}, {-1, 1, 1}}};
    const int tri[12][3] = {{0, 2, 1}, {0, 3, 2}, {4, 5, 6}, {4, 6, 7}, {0, 1, 5}, {0, 5, 4}, {3, 6, 2}, {3, 7, 6}, {0, 4, 7}, {0, 7, 3}, {1, 2, 6}, {1, 6, 5}};
    for (const auto& t : tri) {
        std::array<r::V3, 3> s{}, c{};
        for (int k = 0; k < 3; ++k) {
            const r::V3 p = v[static_cast<std::size_t>(t[k])];
            const r::V4 q = r::mul(mvp, p);
            s[static_cast<std::size_t>(k)] = {(q.x / q.w * 0.5f + 0.5f) * W, (1.0f - (q.y / q.w * 0.5f + 0.5f)) * H, q.z / q.w * 0.5f + 0.5f};
            c[static_cast<std::size_t>(k)] = {(p.x + 1) / 2, (p.y + 1) / 2, (p.z + 1) / 2};
        }
        r::draw(img, s, c);
    }
    if (FILE* f = std::fopen(out, "wb")) {
        std::fprintf(f, "P6\n%d %d\n255\n", W, H);
        std::fwrite(img.rgb.data(), 1, img.rgb.size(), f);
        std::fclose(f);
    }
    std::size_t lit = 0;
    for (std::size_t i = 0; i < img.depth.size(); ++i) lit += img.depth[i] < 1e30f;
    std::printf("imagen=%s %dx%d pixeles_cubiertos=%zu hash=%016llx\n", out, W, H, lit, static_cast<unsigned long long>(r::fnv1a(img.rgb)));
}
