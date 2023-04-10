set(third_part "third_part")
find_program(MAKE_EXE NAMES gmake nmake make)
function(check_solver name)
     set (solver_exist EXISTS "third_part/${name}")
endfunction()




