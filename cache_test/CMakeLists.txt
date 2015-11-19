cmake_minimum_required (VERSION 2.8.1)

project (gearch_cache_test)

aux_source_directory(. DIR_SRCS)

add_executable(cache_cp ${DIR_SRCS})
