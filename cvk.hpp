#pragma once

// ! this is your custom methods and contexts
// ? change this file name to your path to config
// * path should be <>
// #define __cvk_config <cvk_config.hpp>
// you can define this var, if not defined default is cvk_config.hpp
// ? method namespace is not essential for work, but required to provide enum contexts

#include "hpp/contextsstore.h"
#include "hpp/defines.h"
#include "hpp/future.h"
#include "hpp/iocontextgetter.h"
#include "hpp/methodinvoker.h"
#include "hpp/receiver.h"

#include "details/methodinvoker.hpp"
#include "details/receiver.hpp"
#include "details/iocontextgetter.hpp"

// #include "tpp/contextsstore.tpp" // ? included in details/io context getter
#include "tpp/methodinvoker.tpp"
// #include "tpp/receiver.tpp" // ? included in method invoker
