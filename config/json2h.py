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
            sgroup = []

            for i in range(tag["from"], tag["to"] + 1):
                frame = json_info["frames"][i]
                rect = "{ " + \
                       ".x=" + str(frame["frame"]["x"]) + ", " + \
                       ".y=" + str(frame["frame"]["y"]) + ", " + \
                       ".w=" + str(frame["frame"]["w"]) + ", " + \
                       ".h=" + str(frame["frame"]["h"]) +        \
                       " }"
                dt = str(frame["duration"])
                index = str(i)
                sgroup.append( "{ " +                                \
                               ".rect=" + rect + ", " +              \
                               ".dt=" + dt + ", " +                  \
                               ".sheet=" + dest_name.upper() + "," + \
                               ".index=" + index + ", " +            \
                               ".count=" + str(count) +              \
                               " }" )
                tags.append(anim + index)
            sprites.extend(sgroup)

    return ( dest_name.upper(), tags, sprites )

if __name__ == "__main__":
    dest_file = "./src/render_config" # for configuration

    arg_count = len(sys.argv)
    if arg_count < 2:
        print_usage("invalid number of arguments")
        exit(0)

    all_sheets = []
    all_tags = []
    all_sprites = []

    for i in range(1, arg_count):
        if not exists(sys.argv[i]):
            print_usage("not a real json file")
            exit(0)

        sheet, tags, sprites = load_file(sys.argv[i])
        all_sheets.append(sheet)
        all_tags.extend(tags)
        all_sprites.extend(sprites)

    header = []
    source = []
    with open(dest_file + ".h", 'r') as dest:
        autogen = "MAKE AUTOGEN"
        for line in dest:
            header.append(line.rstrip())
            if autogen in line:
                break

        header.append("")
        header.append("/* all sprite sheets */")
        header.append("enum SpriteSheetId {")
        for s in all_sheets:
            header.append("    " + s + ",")
        header.append("    SpriteSheet_COUNT")
        header.append("};")

        header.append("")
        header.append("/* sprite sheet struct */")
        header.append("struct SpriteSheet {")
        header.append("    SDL_Texture *texture;")
        header.append("    i32 w, h;")
        header.append("};")

        header.append("")
        header.append("/* tag mapped to animation */")
        header.append("enum AnimationId {")
        for t in all_tags:
            header.append("    " + t + ",")
        header.append("    Anim_COUNT")
        header.append("};")

        header.append("")
        header.append("/* struct for animations */")
        header.append("struct Animation {")
        header.append("    SDL_Rect rect;")
        header.append("    enum SpriteSheetId sheet;")
        header.append("    u32 dt;")
        header.append("    u32 index;")
        header.append("    u8 count;")
        header.append("};")

        header.append("")
        header.append("/* animations list */")
        header.append("extern struct Animation SPRITES[Anim_COUNT];")

        header.append("")
        header.append("#endif")
        
        source.append("")
        source.append("#include \"" + dest_file.split("/")[-1] + ".h\"")

        source.append("struct Animation SPRITES[Anim_COUNT] = {")
        for s in all_sprites:
            source.append("    " + s + ("," if s != all_sprites[-1] else ""))
        source.append("};")

    with open(dest_file + ".h", 'w') as dest:
        dest.write("\n".join(header))

    with open(dest_file + ".c", "w") as dest:
        dest.write("\n".join(source))

# { rect, dt, anim, index }

