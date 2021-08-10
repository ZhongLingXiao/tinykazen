#include <kazen/common.h>
#include <kazen/frame.h>
#include <kazen/vector.h>
#include <kazen/color.h>
#include <kazen/bbox.h>
#include <kazen/warp.h>
#include <kazen/proplist.h>
using namespace kazen;

int main()
{
    // TODO: 这大哥有点意思
    using Float = Packet<float>;
    KAZEN_BASE_TYPES()
    
    // main test 
    variant::variant<int, float, Vector3f> x;
    x = Vector3f(1, 1, 1);
    std::cout << x.is<Ray3f>() << '\n';

    // // ---------------- warp ----------------
    // Point2f o(0.1, 0.3);
    // auto x = warp::squareToBeckmann(o, Float(1.0f));
    // std::cout << x << '\n';

    // // ---------------- ray & bbox ----------------
    // Point3f min = Point3f(1, 1, 1);
    // Point3f max = Point3f(2, 3, 2);

    // BoundingBox3f box = BoundingBox3f(min, max);
    // BoundingBox3f box1 = BoundingBox3f(min, max);

    // Point3f o(0, 0, 0);
    // Vector3f d(0, 0, 1);
    // Ray3f ray(o, d, 0.5f);

    // auto [tHit, tMin, tMax] = box.rayIntersect(ray);
    // std::cout << tHit << tMin << tMax << '\n';

    // // ---------------- Color ----------------
    // Color3f color = Color3f(1.0f, 0.3f, 0.5);
    // auto srgb = toLinearRGB(color);
    // std::cout << srgb << '\n';

    // // ---------------- Vector && Frame ----------------
    // Vector3f dir = Vector3f(0, 1, 0);
    // Frame3f frame(dir);
    // Vector3f v = Vector3f(0.5f, 1.0f, 0.4f);
    // // std::cout << frame.s << std::endl << frame.t << std::endl;
    // // std::cout << frame.toLocal(v) << std::endl;

    // std::cout << frame << '\n';


    return 0;
}
