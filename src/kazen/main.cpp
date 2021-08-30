#include <kazen/common.h>
#include <kazen/frame.h>
#include <kazen/vector.h>
#include <kazen/color.h>
#include <kazen/bbox.h>
#include <kazen/warp.h>
#include <kazen/proplist.h>
#include <kazen/rfilter.h>
#include <kazen/progress.h>
#include <kazen/block.h>
#include <kazen/sampler.h>
#include <kazen/bitmap.h>
#include <kazen/renderer.h>
#include <kazen/transform.h>

// #include <array>
// #include <tbb/blocked_range.h>
// #include <tbb/parallel_for.h>

using namespace kazen;

int main()
{
    using Float = float;//Packet<float>;
    KAZEN_BASE_TYPES()
    
    // main test 

    std::cout << util::copyright() << '\n';

    // // ---------------- camera ----------------
    // std::cout << ScalarTransform4f() << '\n';

    // auto aspect = 1280/(ScalarFloat)720;
    // auto sampleToCamera = 
    //         ScalarTransform4f::scale(Vector3f(-0.5f, -0.5f * aspect, 1.f)) *
    //         ScalarTransform4f::translate(Vector3f(-1.f, -1.f / aspect, 0.f)) * 
    //         ScalarTransform4f::perspective(50.0, 0.1, 10000.0);

    // std::cout << sampleToCamera << '\n';

    // // ---------------- Render ----------------
    // Renderer renderer;
    // renderer.render(nullptr, "kazen");

    // // ---------------- image block ----------------
    // auto size = Vector2i(60, 60);
    // auto b = BlockGenerator(size, KAZEN_BLOCK_SIZE);

    // PropertyList list;
    // list.setInt("index", 4);
    // ReconstructionFilter* filter = static_cast<ReconstructionFilter*>(ObjectFactory::createInstance("tent", list));
    // ImageBlock image(size, filter);
    // ImageBlock block(Vector2i(KAZEN_BLOCK_SIZE), filter);
    // image.clear();
    // block.clear();
    
    // // put
    // image.put(block);

    // auto png = image.toBitmap();
    // png->savePNG("foo");

    // // fmt::print("[kazen debug]: {}", debugInfo);

    // // next()
    // // std::cout << b.getBlockCount() << '\n';
    // // std::cout << (b.next(block)? "true" : "false") << block.getSize() << '\n';
    // // std::cout << b.getBlockCount() << '\n';
    // // std::cout <<  (b.next(block)? "true": "false") << block.getSize() << '\n';
    // // std::cout << b.getBlockCount() << '\n';
    // // std::cout <<  (b.next(block)? "true" : "false") << block.getSize() << '\n';
    // // std::cout << b.getBlockCount() << '\n';
    // // std::cout <<  (b.next(block)? "true" : "false") << block.getSize() << '\n';
    // // std::cout << b.getBlockCount() << '\n';
    // // std::cout <<  (b.next(block)? "true" : "false") << '\n'; // false
    
    // if (filter)
    //     delete filter;

    // // ---------------- sampler ----------------
    // PropertyList list;
    // list.setInt("sampleCount", 4);
    // Sampler* result = static_cast<Sampler*>(ObjectFactory::createInstance("independent", list));
    // std::cout << result->next1D() << '\n';

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
