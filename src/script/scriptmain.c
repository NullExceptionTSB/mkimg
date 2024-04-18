#include <arg.h>
#include <fail.h>

#include <script/scriptfile.h>
#include <script/scriptparser.h>

int script_main(mkimg_args* args) {
    fassert(args->scriptfile!=NULL, "F: No script file specified");
    fassert(args->outfile!=NULL, "F: No output file specified");

    scriptfile* script = scriptfile_new(args->scriptfile);
    fassert(script!=NULL, "F: Failed to open script file");

    FILE* outfile = fopen(args->outfile, "wb");
    fassert(outfile!=NULL, "F: Failed to open output file");

    script_run(script, outfile);
}