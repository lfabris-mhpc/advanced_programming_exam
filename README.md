# advanced_programming_exam

# Compilation
To compile the code, run make in the repository's directory. The default compile flags are -Wall -Wextra; other flags can be appended with make CXXFLAGS="..." LDFLAGS="..."

The first program that will be built is test.x, which tests the various constructors, insertion methods, and the balance mechanism of bst.
All operations are performed on a bst using the std::less comparator, another using std::greater, and a std::map; the content of the containers are printed after each action.

The second program is profile.x, which performs insertions and random searches on either an std::map or bst.
The usage is: ./profile.x stdmap|bst|bst_unbalanced (#random_insertions default: 1000000) (#searches default: 1000000) (seed_insert default: 123543) (seed_search default: 874563)
Random keys are generated from a uniform distribution and inserted until the specified container size is reached.
Then searches for random keys are performed; if the container is bst, it is balanced and the searches are performed again. 
The key type is std::size_t, KEY_SIZE> where KEY_SIZE is a macro (default 1); to test different sizes, rebuild the program.
Only the last element of the key is random, so that KEY_SIZE-1 comparisons are performed anyway.
