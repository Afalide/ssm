
#pragma once

#define TINYSM_TEST_THREAD_COUNT 8
#define TINYSM_TEST_THREAD_PULSES 5

void do_all_tests();

// Protected resource test
// Makes sure only one thread is accessing the resource at a time

void test1();

// State machine tests

void test2();

