#!/bin/python
import json
import sys
from os.path import exists

def print_usage(msg=None):
    if msg:
        print(msg)
    print("json2h.py [sprite.json]+")

def load_file(filename):
    dest_name = filename[filename.rfind('/') + 1:filename.rfind('.')]
    tags = []
    sprites = []

    with open(filename) as json_file:
        json_info = json.loads(json_file.read())

        for tag in json_info["meta"]["frameTags"]:
            anim = (dest_name + "_" + tag["name"]).upper()
            count = tag["to"] - tag["from"] + 1
            tags.append(anim)
            sgroup = []

            for i in range(tag["from"], tag["to"] + 1):
                frame = json_info["frames"][i]
                rect = "{ " + \
                       "x=" + str(frame["frame"]["x"]) + ", " + \
                       "y=" + str(frame["frame"]["y"]) + ", " + \
                       "w=" + str(frame["frame"]["w"]) + ", " + \
                       "h=" + str(frame["frame"]["h"]) +        \
                       " }"
                dt = str(frame["duration"])
                index = str(i)
                sgroup.append( "{ " +                    \
                               "rect=" + rect + ", " +   \
                               "dt=" + dt + ", " +       \
                               "anim=" + anim + ", "     \
                               "index=" + index + ", " + \
                               "count=" + str(count) +   \
                               " }" )
            sprites.append(sgroup)

    return ( tags, sprites )

if __name__ == "__main__":
    base_path = "./config/" # for configuration
    dest_file = "./src/config.h" # for configuration

    arg_count = len(sys.argv)
    if arg_count < 2:
        print_usage("invalid number of arguments")
        exit(0)

    all_tags = []
    all_sprites = []

    for i in range(1, arg_count):
        if not exists(base_path + sys.argv[i]):
            print_usage("not a real json file")
            exit(0)

        tags, sprites = load_file(base_path + sys.argv[i])
        all_tags.extend(tags)
        all_sprites.extend(sprites)

    lines = []
    with open(dest_file, 'r') as dest:
        autogen = "MAKE AUTOGEN"
        for line in dest:
            lines.append(line.rstrip())
            if autogen in line:
                break

        lines.append("")
        lines.append("/* tag mapped to animation */")
        lines.append("enum AnimId {")

        for t in all_tags:
            lines.append("    " + t + ",")
        lines.append("    Anim_COUNT")
        lines.append("};")

        lines.append("")
        lines.append("/* struct for animations */")
        lines.append("struct Animation {")
        lines.append("    SDL_Rect rect;")
        lines.append("    u32 dt;")
        lines.append("    enum AnimId anim;")
        lines.append("    u32 index;")
        lines.append("    u8 count;")
        lines.append("};")

        lines.append("")
        lines.append("/* animations list */")
        lines.append("struct Animation sprites[][] = {")
        for s in all_sprites:
            lines.append("    {")
            temp = (",~".join(s)).split("~")
            for t in temp:
                lines.append("      " + t)
            lines.append("    }" + ("," if s != all_sprites[-1] else ""))
        lines.append("};")

    with open(dest_file, 'w') as dest:
        dest.write("\n".join(lines))

# { rect, dt, anim, index }

