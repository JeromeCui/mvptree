#ifndef MVPTREE_GO_H
#define MVPTREE_GO_H

#include <cstdlib>
#include <cstring>
#include <vector>
#include <string>
#include <sys/stat.h>

#include "mvptree.h"


namespace GoMVPTree {
    /**
     * An MVPTree wrap for golang
     */
    class MVPTreeWrap {
    public:
        MVPTreeWrap(const char *filename, const int branchFactor, const int pathLength, const int leafCap)
            : mTreeFilename(filename),
            mBranchFactor(branchFactor),
            mPathLength(pathLength),
            mLeafCap(leafCap){
        }

        /**
         * Initialize the tree from disk or create new one.
         */
        int init() {
            struct stat buf;
            bool fileExists = stat(mTreeFilename, &buf) == 0;
            if (fileExists) {
                MVPError err;
                mTree = mvptree_read(mTreeFilename, HammingDistanceComp, mBranchFactor, mPathLength, mLeafCap, &err);
                if (err != MVP_SUCCESS) {
                    mvptree_clear(mTree, free);
                    mTree = NULL;
                    return int(err);
                }
                
                return MVP_SUCCESS;
            } else {
                mTree = mvptree_alloc(NULL, HammingDistanceComp, mBranchFactor, mPathLength, mLeafCap);
                return int(MVP_SUCCESS);
            }
        }

        /**
         * Add new image item to this tree
         */
        int add(const char *imgId, const uint64_t hash) {
            MVPDP *point = dp_alloc(UINT64ARRAY);
            point->id = strdup(imgId);
            point->data = malloc(1*UINT64ARRAY);
            point->datalen = 1;
            memcpy(point->data, &hash, UINT64ARRAY);

            return mvptree_add(mTree, &point, 1);
        }

        /**
         * Query nearest images to target image
         */
        std::vector<std::string> query(const uint64_t targetHash, const int kNearest, const float radius, int *errCode) {
            MVPDP *point = dp_alloc(UINT64ARRAY);
            point->id = "";
            point->data = malloc(1*UINT64ARRAY);
            point->datalen = 1;
            memcpy(point->data, &targetHash, UINT64ARRAY);

            MVPError err;
            unsigned int numRes;
            MVPDP **results = mvptree_retrieve(mTree, point, kNearest, radius, &numRes, &err);
            if (err != MVP_SUCCESS) {
                *errCode = int(err);
                return std::vector<std::string>();
            }

            std::vector<std::string> imgIds;
            imgIds.reserve(numRes);
            for (int i = 0; i < numRes; i++) {
                MVPDP *point = results[i];
                imgIds.push_back(std::string(point->id));
            }
            free(results);

            return imgIds;
        }

        /**
         * save tree to disk
         */
        int save() {
            MVPError error = mvptree_write(mTree, mTreeFilename, 00755);
            return int(error);
        }

        ~MVPTreeWrap() {
            mvptree_clear(mTree, free);
            mTree = NULL;
        }

    private:
        const char *mTreeFilename;
        const int mBranchFactor;
        const int mPathLength;
        const int mLeafCap;
        MVPTree *mTree;
    };

    static inline float HammingDistanceComp(MVPDP *pointA, MVPDP *pointB) {
        if (!pointA || !pointB || pointA->datalen != pointB->datalen) {
            return -1.0f;
        }
    
        uint64_t a = *((uint64_t*) pointA->data);
        uint64_t b = *((uint64_t*) pointB->data);
    
        uint64_t x = a^b;
        const uint64_t m1  = 0x5555555555555555ULL;
        const uint64_t m2  = 0x3333333333333333ULL;
        const uint64_t h01 = 0x0101010101010101ULL;
        const uint64_t m4  = 0x0f0f0f0f0f0f0f0fULL;
        x -= (x >> 1) & m1;
        x = (x & m2) + ((x >> 2) & m2);
        x = (x + (x >> 4)) & m4;
    
        return (float)((x*h01)>>56);
    };
}

#endif