#include <kazen/common.h>
#include <kazen/frame.h>
#include <kazen/vector.h>
#include <kazen/color.h>
#include <kazen/bbox.h>
#include <kazen/warp.h>
#include <kazen/proplist.h>
#include <kazen/rfilter.h>
#include <kazen/progress.h>

using namespace kazen;
#include <thread>
#include <chrono> 
int main()
{
    // using Float = Packet<float>;
    // KAZEN_BASE_TYPES()
    
    // main test 






    // // ---------------- Progressbar ----------------
    // auto p = Progress("Rendering...");
    // for (int i = 0; i <= 100; i++) {
    //     std::this_thread::sleep_for(std::chrono::milliseconds(50));
    //     p.update(i/100.0);
    // }

    // // ---------------- Object ----------------
    // PropertyList list;
    // list.setInt("index", 15);
    // auto result = ObjectFactory::createInstance("tent", list);
    // std::cout << result->toString() << '\n';

    // // ---------------- PropertyList ----------------
    // PropertyList list;
    // list.setInt("index", 15);
    // list.setFloat("base", 0.35f);
    // list.setColor3f("color", ScalarColor3f(1.0, 0.2, 0.5));
    // std::cout << list << '\n';

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
