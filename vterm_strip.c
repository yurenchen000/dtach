// vi: ts=4 sw=4 
/*
A cli tool to strip terminal output, only keep final text with color (which show on screen)

 build: gcc vterm_strip.c -o vterm_strip -lvterm
depend: apt install libvterm-dev
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <vterm.h>

// Callback to handle terminal damage (redraw events)
static int damage(VTermRect rect, void *user) {
    // We don't need to redraw graphically, so just return 0
    return 0;
}

// Callback to handle terminal bell
static int bell(void *user) {
    return 0;
}

// Callback to set terminal title (ignored)
static int settermprop(VTermProp prop, VTermValue *val, void *user) {
    return 0;
}

/*
  VTERM_COLOR_RGB = 0x00,
  VTERM_COLOR_INDEXED = 0x01,
  VTERM_COLOR_DEFAULT_FG = 0x02,
  VTERM_COLOR_DEFAULT_BG = 0x04,
*/

// Convert VTermColor to ANSI escape code
static int color_to_ansi(VTermColor *color, char *buffer, int is_fg) {
	// fprintf(stderr, "color: %d\n", color->type);

    //// checked in caller
    // if (!color || color->type == (is_fg?VTERM_COLOR_DEFAULT_FG:VTERM_COLOR_DEFAULT_BG)) {
    //     // Default or invalid color: No escape code
    //     buffer[0] = '\0';
    //     return 0;
    // }

    if (color->type == VTERM_COLOR_INDEXED) {
        uint8_t index = color->indexed.idx;
        // fprintf(stderr, "color idx: %d\n", color->indexed.idx);
        if (index < 8) {
            // Basic 8 colors (0=black, 1=red, etc.)
            sprintf(buffer, "\033[%dm", is_fg ? 30 + index : 40 + index);
            return 1;
        } else {
            // 256-color palette
            sprintf(buffer, "\033[%d;5;%dm", is_fg ? 38 : 48, index);
            return 1;
        }
        // Optional: Approximate indexed colors to RGB (uncomment to enable)
        /*
        static const struct { uint8_t r, g, b; } palette[8] = {
            {0, 0, 0},    // Black
            {255, 0, 0},  // Red
            {0, 255, 0},  // Green
            {255, 255, 0},// Yellow
            {0, 0, 255},  // Blue
            {255, 0, 255},// Magenta
            {0, 255, 255},// Cyan
            {255, 255, 255}// White
        };
        if (index < 8) {
            sprintf(buffer, "\033[%d;2;%d;%d;%dm", is_fg ? 38 : 48,
                    palette[index].r, palette[index].g, palette[index].b);
            return 1;
        }
        // For indices 8–255, use 256-color code or a palette lookup
        sprintf(buffer, "\033[%d;5;%dm", is_fg ? 38 : 48, index);
        return 1;
        */
    } else

    if (color->type == VTERM_COLOR_RGB) {
        // fprintf(stderr, "color rgb: %d %d %d\n", color->rgb.red, color->rgb.green, color->rgb.blue);
        // Truecolor (RGB) directly from color->rgb
        sprintf(buffer, "\033[%d;2;%d;%d;%dm", is_fg ? 38 : 48,
                color->rgb.red, color->rgb.green, color->rgb.blue);
        return 1;
    } else

    if (color->type == VTERM_COLOR_DEFAULT_FG)
    {
        // fprintf(stderr, "=color fg: %d\n", color->type);
        if(is_fg) printf("\033[0m");  //TODO: this also reset BG, which not we want
        // if(is_fg) printf("_");
    } else

    if (color->type == VTERM_COLOR_DEFAULT_BG)
    {
        // fprintf(stderr, "=color bg: %d\n", color->type);
        // if(!is_fg) printf("\033[0m");
        // if(!is_fg) printf("?");
    }

    // Unknown color type: No escape code
    buffer[0] = '\0';
    return 0;
}

static int print_color(VTermColor *color) {
	// fprintf(stderr, "color: %d\n", color->type);

    if (color->type == VTERM_COLOR_INDEXED) {
        uint8_t index = color->indexed.idx;
        fprintf(stderr, "color idx: %d\n", color->indexed.idx);
    }else
    if (color->type == VTERM_COLOR_RGB) {
        fprintf(stderr, "color rgb: %d %d %d\n", color->rgb.red, color->rgb.green, color->rgb.blue);
    }else {
        fprintf(stderr, "color: %d\n", color->type);
    }
}

//ret 1 if eq
static int color_equal(const VTermColor *a, const VTermColor *b) {
    if (a->type != b->type) return 0;
    // if (a->type == VTERM_COLOR_INVALID) return 1;
    if (a->type == VTERM_COLOR_INDEXED)
        return a->indexed.idx == b->indexed.idx;
    if (a->type == VTERM_COLOR_RGB) { //ret 1 if eq
        return a->rgb.red == b->rgb.red &&
               a->rgb.green == b->rgb.green &&
               a->rgb.blue == b->rgb.blue;
    }
    if (a->type == VTERM_COLOR_DEFAULT_FG) {
        return a->type == b->type;
    }
    if (a->type == VTERM_COLOR_DEFAULT_BG) {
        return a->type == b->type;
    }

    return 0;
}

// Unicode codepoint => UTF-8
int utf8_encode(uint32_t codepoint, char *out) {
    if (codepoint <= 0x7F) {
        out[0] = codepoint;
        return 1;
    } else if (codepoint <= 0x7FF) {
        out[0] = 0xC0 | (codepoint >> 6);
        out[1] = 0x80 | (codepoint & 0x3F);
        return 2;
    } else if (codepoint <= 0xFFFF) {
        out[0] = 0xE0 | (codepoint >> 12);
        out[1] = 0x80 | ((codepoint >> 6) & 0x3F);
        out[2] = 0x80 | (codepoint & 0x3F);
        return 3;
    } else if (codepoint <= 0x10FFFF) {
        out[0] = 0xF0 | (codepoint >> 18);
        out[1] = 0x80 | ((codepoint >> 12) & 0x3F);
        out[2] = 0x80 | ((codepoint >> 6) & 0x3F);
        out[3] = 0x80 | (codepoint & 0x3F);
        return 4;
    }
    return 0;
}

void get_term_size_from_env(int *_rows, int *_cols) {
    const char *lines_str = getenv("LINES");
    const char *cols_str = getenv("COLUMNS");
    // fprintf(stderr, "=env size row, col: %s, %s\n", lines_str, cols_str);

    int rows, cols;
    if (cols_str) {
        cols = atoi(cols_str);
        if(cols>0) *_cols = cols;
    }
    if (lines_str) {
        rows = atoi(lines_str);
        if(rows>0) *_rows = rows;
    }
}

// Main function
int main() {
    int rows = 999, cols = 80;  //default size
    get_term_size_from_env(&rows, &cols);

    // Initialize libvterm
    // VTerm *vterm = vterm_new(999, 80); // 999 rows, 80 columns
    VTerm *vterm = vterm_new(rows, cols);
    fprintf(stderr, "=set size row, col: %d, %d\n", rows, cols);
    if (!vterm) {
        fprintf(stderr, "Failed to create vterm\n");
        return 1;
    }
    vterm_set_utf8(vterm, 1);

    // Enable screen features
    VTermScreen *screen = vterm_obtain_screen(vterm);
    vterm_screen_enable_altscreen(screen, 1);
    vterm_screen_reset(screen, 1);

    // Set callbacks
    VTermScreenCallbacks callbacks = {
        .damage = damage,
        .bell = bell,
        .settermprop = settermprop,
    };
    vterm_screen_set_callbacks(screen, &callbacks, NULL);

    // Set input to expect raw sequences
    vterm_set_utf8(vterm, 1);

    // Read raw terminal sequences from stdin
    char buffer[1024];
    size_t len;
    while ((len = fread(buffer, 1, sizeof(buffer), stdin)) > 0) {
        vterm_input_write(vterm, buffer, len);
        vterm_screen_flush_damage(screen);
    }

    // Get terminal dimensions
    VTermPos pos;
    vterm_get_size(vterm, &pos.row, &pos.col);

    // Extract and output screen content
    char ansi_buffer[64];
    VTermScreenCell cell;
    VTermColor last_fg,last_bg;
    int count_empty_row = 0;
    for (int row = 0; row < pos.row; row++) {
        int is_empty_row = 1;
        for (int col = 0; col < pos.col; col++) {
            VTermPos cell_pos = {row, col};
            vterm_screen_get_cell(screen, cell_pos, &cell);

            // Skip empty cells (no text or attributes)
            if (cell.chars[0] == 0 && cell.attrs.bold == 0 && cell.attrs.underline == 0 &&
                cell.attrs.italic == 0 && cell.attrs.blink == 0 && cell.attrs.reverse == 0 &&
                cell.attrs.strike == 0 && cell.attrs.font == 0) {
                continue;
            }

            is_empty_row = 0;

            // Reset previous attributes
            // printf("\033[0m");
            // print_color(&cell.fg);

            // Set foreground color
            // if (cell.fg.type != VTERM_COLOR_DEFAULT_FG && color_equal(&cell.fg, &last_fg)) {
            if (!color_equal(&cell.fg, &last_fg)) {
                color_to_ansi(&cell.fg, ansi_buffer, 1);
                printf("%s", ansi_buffer);
            }

            // Set background color
            // if (cell.bg.type != VTERM_COLOR_DEFAULT_BG && color_equal(&cell.bg, &last_bg)) {
            if (!color_equal(&cell.bg, &last_bg)) {
                color_to_ansi(&cell.bg, ansi_buffer, 0);
                printf("%s", ansi_buffer);
            }
            last_fg = cell.fg;
            last_bg = cell.bg;

            // Output text (handle multi-byte characters)
            if (cell.chars[0] != 0) {
                // char out[8];
                char out[8*4] = {0};
                // int i = 0;
                char *p=&out[0];
                for (int j = 0; j < VTERM_MAX_CHARS_PER_CELL && cell.chars[j] != 0; j++) {
                    // out[i++] = cell.chars[j] & 0xFF; // Simplified UTF-8 handling
                    int len = utf8_encode(cell.chars[j], p);
                    p+=len;
                }
                *p = '\0';
                // out[i] = '\0';
                // fprintf(stderr, "%s\n", out);
                printf("%s", out);
            }
        } //for cols
        if (!is_empty_row) {
            // printf("\033[0m\n"); // Reset and newline for non-empty rows
            printf("\n");
        }else { //empty row
            if(!count_empty_row){
                // printf("r%d\n", row); //trim multi empty line
                // printf("\n");
            }
        }
        //hack: trim multi empty line
        if (is_empty_row){
            count_empty_row++;
            // printf("r%d: %d\n", row, count_empty_row); //trim multi empty line
            if(count_empty_row==3)
                printf("\n\n");
        }else{
            count_empty_row=0;
        }

    } //for rows

    // Clean up
    vterm_free(vterm);
    return 0;
}

