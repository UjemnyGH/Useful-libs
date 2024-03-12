# Useful libraries
These are single file libraries for everything!
Ok, maybe almost everything.

## Remember that this libraries are not free of bugs, I assume you propably can find a lot of memory leaks.

<!--
STB have nice table, so I firmly got inspired by it`s design
-->

|              Lib               |   Category    |       Description        |
|--------------------------------|:-------------:|:------------------------:|
|[ul_audio.h](ul_audio.h)        | audio         | read (later also save) wave (maybe later also mp3) files |
|[ul_sound.h](ul_sound.h)        | audio         | wraps audio APIs (currently only [ALSA](https://github.com/alsa-project))
|[ul_mesh.h](ul_mesh.h)          | 3D geometry   | Loads PLY/OBJ/STL meshes |
|[ul_quick_gui.h](ul_quick_gui.h)| User interface| Create simple user interface (unfinished, don`t use) |
|[ul_bitmap.h](ul_bitmap.h)      | Images processing | Load BMP files      |

<hr>

#### These are loosely made libraries, so some of them may be for specific operating system, some of them may only operate on a few cases of formats/things.

Idea to make some sort of single-file libraries comes from [stb](https://github.com/nothings/stb) collection of single-file libraries. I created those libraries mainly to extend [stb](https://github.com/nothings/stb) libraries which I thought were missing for me there.

<hr>
I probably gonna create more when I am gonna need it or feel urge to create some more.

Have fun!