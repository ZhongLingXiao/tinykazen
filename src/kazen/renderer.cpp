#include <kazen/renderer.h>

#include <kazen/proplist.h>
#include <kazen/progress.h>
#include <kazen/block.h>
#include <kazen/rfilter.h>

#include <tbb/tbb.h>
#include <tbb/parallel_for.h>
#include <tbb/blocked_range.h>


#include <thread>
#include <mutex>

NAMESPACE_BEGIN(kazen)

void Renderer::renderSample(const Scene *scene, Sampler *sampler, ImageBlock &block, const Vector2f &pos, Mask active) {
    // TODO
}

void Renderer::renderBlock(const Scene *scene, Sampler *sampler, ImageBlock &block) {
    #include <unistd.h>
    usleep(1000000);
}


void Renderer::render(Scene *scene, const std::string &filename) {

    // FIXME
    ScalarVector2i outputSize(1000, 1000);
    
    /* Create a block generator (i.e. a work scheduler) */
    BlockGenerator blockGenerator(outputSize, KAZEN_BLOCK_SIZE);

    /* Allocate memory for the entire output image and clear it */
    PropertyList list;
    list.setInt("index", 4);
    ReconstructionFilter* filter = static_cast<ReconstructionFilter*>(ObjectFactory::createInstance("tent", list));
    ImageBlock result(outputSize, filter);
    result.clear();

    /* Do the following in parallel and asynchronously */
    std::thread render_thread([&] {

        auto progress = Progress("Rendering...");
        std::mutex mutex;

        // Total number of blocks to be handled, including multiple passes.
        size_t totalBlocks = blockGenerator.getBlockCount();
        size_t blocksDone = 0;

        tbb::blocked_range<int> range(0, blockGenerator.getBlockCount());

        auto map = [&](const tbb::blocked_range<int> &range) {
            /* Allocate memory for a small image block to be rendered by the current thread */
            PropertyList list;
            list.setInt("index", 4);
            ReconstructionFilter* filter = static_cast<ReconstructionFilter*>(ObjectFactory::createInstance("tent", list));            
            ImageBlock block(ScalarVector2i(KAZEN_BLOCK_SIZE), filter);

            /* Create a clone of the sampler for the current thread */
            // std::unique_ptr<Sampler> sampler(scene->getSampler()->clone());

            for (int i=range.begin(); i<range.end(); ++i) {
                /* Request an image block from the block generator */
                blockGenerator.next(block);

                // std::this_thread::sleep_for(std::chrono::milliseconds(1000));
               renderBlock(scene, nullptr, block);

                /* The image block has been processed. Now add it to
                   the "big" block that represents the entire image */
                result.put(block);

                /* Critical section: update progress bar */ {
                    std::lock_guard<std::mutex> lock(mutex);
                    blocksDone++;
                    progress.update(blocksDone / (ScalarFloat) totalBlocks);
                }
            }
        };

        /// Default: parallel rendering
        tbb::parallel_for(range, map);
    });

    /* Shut down the user interface */
    render_thread.join();


}

NAMESPACE_END(kazen)
