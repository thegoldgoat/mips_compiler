project('mips_emulator', 'cpp')
executable('assemble', 'src/assemble.cpp', \
    'src/Assembler/Assembler.cpp', \
    'src/Assembler/Utils.cpp',     \
    'src/Common/Symbol.cpp',     \
    install: true
)

executable('link', 'src/link.cpp',  \
    'src/Linker/Linker.cpp',        \
    'src/Common/Symbol.cpp',        \
    'src/Linker/Utils.cpp',         \
    install: true
)