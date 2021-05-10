# ################################################################
# Copyright (c) 2015-present, Yann Collet, Facebook, Inc.
# All rights reserved.
#
# This source code is licensed under both the BSD-style license (found in the
# LICENSE file in the root directory of this source tree) and the GPLv2 (found
# in the COPYING file in the root directory of this source tree).
# ################################################################



file(GLOB CommonSources ${CMAKE_SOURCE_DIR}/third_party/zstd/common/*.c)
file(GLOB CompressSources ${CMAKE_SOURCE_DIR}/third_party/zstd/compress/*.c)
file(GLOB DecompressSources ${CMAKE_SOURCE_DIR}/third_party/zstd/decompress/*.c)
file(GLOB DictBuilderSources ${CMAKE_SOURCE_DIR}/third_party/zstd/dictBuilder/*.c)
file(GLOB DeprecatedSources ${CMAKE_SOURCE_DIR}/third_party/zstd/deprecated/*.c)

set(Sources
        ${CommonSources}
        ${CompressSources}
        ${DecompressSources}
        ${DictBuilderSources}
        ${DeprecatedSources})

file(GLOB CommonHeaders ${CMAKE_SOURCE_DIR}/third_party/zstd/common/*.h)
file(GLOB CompressHeaders ${CMAKE_SOURCE_DIR}/third_party/zstd/compress/*.h)
file(GLOB DecompressHeaders ${CMAKE_SOURCE_DIR}/third_party/zstd/decompress/*.h)
file(GLOB DictBuilderHeaders ${CMAKE_SOURCE_DIR}/third_party/zstd/dictBuilder/*.h)
file(GLOB DeprecatedHeaders ${CMAKE_SOURCE_DIR}/third_party/zstd/deprecated/*.h)

set(Headers
        ${CMAKE_SOURCE_DIR}/third_party/zstd/zstd.h
        ${CommonHeaders}
        ${CompressHeaders}
        ${DecompressHeaders}
        ${DictBuilderHeaders}
        ${DeprecatedHeaders})

add_library(zstd STATIC ${Sources} ${Headers})
target_link_libraries(zstd z)





