function Screen(canvas_id) {
    this.default_cell_attributes = new CellAttributes();
    this.default_cell_attributes.italic = false;
    this.default_cell_attributes.bold = false;
    this.default_cell_attributes.fg_color = 'white';

    //this.canvas = document.getElementById("terminal");
    this.canvas = $('#' + canvas_id)[0];
    this.ctx = this.canvas.getContext("2d");
    
    this.font_base = '16px Courier New';
    
    // I must apply attributes (font basically) to calculate font size 
    this.default_cell_attributes.apply(this.ctx, this.font_base);

    this.wrap_around = false;
    this.cell_width = this.ctx.measureText('M').width;
    this.cell_height = getFontHeight('Courier New', '16px', 'Mg');
    
    this.width = 120;
    this.height = 40;

    this.empty_row = '';    

    for (var x=0; x <= this.width; x++) {
        this.empty_row += ' ';
    }

    this.canvas.width = this.cell_width * this.width;
    this.canvas.height = this.cell_height.height * this.height;

    this.ctx.fillStyle = this.default_cell_attributes.fg_color;
    
    this.tmp_canvas = document.createElement('canvas');
    this.tmp_canvas.width = this.canvas.width;
    this.tmp_canvas.height = this.canvas.height;
    this.tmp_ctx = this.tmp_canvas.getContext('2d');
    this.tmp_canvas.visible = false;
      
    // after resizing, I must reapply attributes
    this.default_cell_attributes.apply(this.ctx, this.font_base);
    
    // for cursor
    this.ctx.strokeStyle = this.default_cell_attributes.fg_color;
    
    this.scroll_region = {first:0, last:this.height-1};
    
    this.cursor = {x:0, y:0};
    
    this.default_bg_color = $("body").css("background-color");
    
    this.chars_buffer = '';
    
    this.set_default_attributes();
    
    this.ctx_cell_attributes = undefined;
    
    this.colors = true;
    
    this.clear(true);
    
    this.scroll_back_buffer = new Array();
    
    this.scroll_back_line = 0;
    
    this.scrolling = false;
    
    this.cursor_visible = true;
};

Screen.prototype.canvas;
Screen.prototype.ctx;
// x,y
Screen.prototype.cursor;
Screen.prototype.cell_width;
Screen.prototype.cell_height;
Screen.prototype.width;
Screen.prototype.height;
// array of strings, indexed by row
Screen.prototype.cells;
Screen.prototype.wrap_around;
//Screen.prototype.default_bg_color;
//Screen.prototype.bg_color;
// first,last
Screen.prototype.scroll_region;
// true / false
Screen.prototype.colors;
// string, size ' ' family
Screen.prototype.font_base;
// CellAttributes
Screen.prototype.default_cell_attributes;
// CellAttributes: current attributes (may be or not applied to the context) 
Screen.prototype.cell_attributes;
// CellAttributes: attributes applied to the context
Screen.prototype.ctx_cell_attributes;
// a row of spaces
Screen.empty_row;
Screen.chars_buffer;
// array of arrays of CellAttributes, indexed by row,column
Screen.prototype.cells_attributes;
Screen.prototype.tmp_canvas;
Screen.prototype.tmp_ctx;
// true if cursor is visible, false otherwise
Screen.prototype.cursor_visible;
Screen.prototype.updated;
// an array of strings
Screen.prototype.scroll_back_buffer;
// the top line (of buffer) to show when scrolling
Screen.prototype.scroll_back_line;
// {from:{x,y}, to:{x,y}}
Screen.prototype.selection;
Screen.prototype.selected_text;

Screen.prototype.get_coords_at_pixel = function(x, y) {
    return {
        x: Math.max(0, Math.min(this.width -1, Math.floor(x / this.cell_width))), 
        y: Math.max(0, Math.min(this.height - 1, Math.floor(y / this.cell_height.height)))
    };
};

// {from:{x,y}, to{x,y}}
Screen.prototype.set_selection = function(selection) {
    this.selection = selection;
    this.updated = false;
    //log('selection={from:' + this.selection.from.x + ',' + this.selection.from.y + ',to:' + this.selection.to.x + ',' + this.selection.to.y, WARN);
    if (selection) {
        this.selected_text = '';
        for (var y = this.selection.from.y; y <= this.selection.to.y; y++) {
            if (y >= 0 && y < this.height) {
                var from_x = 0;
                var to_x = this.width -1;
                if (y == this.selection.from.y) {
                    from_x = this.selection.from.x;
                }
                if (y == this.selection.to.y) {
                    to_x = this.selection.to.x;
                }
                if (this.selected_text.length > 0) {
                    this.selected_text += '\n';
                }
                if (this.scrolling) {
                    var line;
                    if (this.scroll_back_buffer.length > this.scroll_back_line + y) {
                        line = this.scroll_back_buffer[this.scroll_back_line + y]; 
                    } else {
                        line = this.cells[y - (this.scroll_back_buffer.length - this.scroll_back_line)];  
                    }
                    this.selected_text += line.substr(from_x, to_x - from_x + 1);
                } else {
                    this.selected_text += this.cells[y].substr(from_x, to_x - from_x + 1);
                }
            }
        }
        if (this.selected_text.length == 0) {
            this.selected_text = undefined;
//        } else {
//            log('selected text="' + this.selected_text + '"', WARN);
        }
    } else {
        this.selected_text = undefined;
    }
};

Screen.prototype.get_selected_text = function() {
    return this.selected_text;
};

Screen.prototype.scroll_back_page_up = function() {
    this.scroll_back_up(this.height);
};

Screen.prototype.scroll_back_page_down = function() {
    this.scroll_back_down(this.height);
};

Screen.prototype.scroll_back_up = function(lines) {
    var real_scroll = lines;

    this.scroll_back_line -= lines;
    
    if (this.scroll_back_line < 0) {
        real_scroll += this.scroll_back_line;
        this.scroll_back_line = 0;
    }
    
    if (this.selection) {
        this.selection.from.y += real_scroll;
        this.selection.to.y += real_scroll;
    }
   
    this.scrolling = true;

    if (this.scroll_back_line == this.scroll_back_buffer.length) {
        this.reset_scrolling();
    }
       
    this.updated = false;
};

Screen.prototype.scroll_back_down = function(lines) {
    var real_scroll = lines;
    this.scroll_back_line += lines;
    
    if (this.scroll_back_line > this.scroll_back_buffer.length) {
        real_scroll -= this.scroll_back_line - this.scroll_back_buffer.length;
        this.scroll_back_line = this.scroll_back_buffer.length;
    } else {
        this.scrolling = true;
    }
    
    if (this.selection) {
        this.selection.from.y -= real_scroll;
        this.selection.to.y -= real_scroll;
    }
    
    if (this.scroll_back_line == this.scroll_back_buffer.length) {
        this.reset_scrolling();
    }
    this.updated = false;
};

Screen.prototype.reset_scrolling = function() {
    this.flush();
    this.scrolling = false;
    this.scroll_back_line = this.scroll_back_buffer.length;
};

Screen.prototype.update = function() {
    if (!this.updated) {
//        this.canvas.width = this.canvas.width;
        this.ctx.clearRect(0, 0, this.canvas.width, this.canvas.height);
        
        if (this.scrolling) {
            for (var y = 0; y < this.height; y++) {
                var line;
                
                if (this.scroll_back_buffer.length > this.scroll_back_line + y) {
                    line = this.scroll_back_buffer[this.scroll_back_line + y]; 
                } else {
                    line = this.cells[y - (this.scroll_back_buffer.length - this.scroll_back_line)];  
                }
                this.ctx.fillText(line, 0, y * this.cell_height.height + 
                    this.cell_height.ascent);
            }
        } else {
            this.draw(this.ctx);
        }
        this.updated = true;
        
        // TODO optimize
        this.ctx.globalCompositeOperation = 'xor';
        
        if (this.cursor_visible && !this.scrolling) {
            this.ctx.fillRect(this.cursor.x * this.cell_width, this.cursor.y * this.cell_height.height,
                this.cell_width, this.cell_height.height);
        }
        
        if (this.selection) {
            for (var y = this.selection.from.y; y <= this.selection.to.y; y++) {
                if (y >= 0 && y < this.height) {
                    var from_x = 0;
                    var to_x = this.width -1;
                    if (y == this.selection.from.y) {
                        from_x = this.selection.from.x;
                    }
                    if (y == this.selection.to.y) {
                        to_x = this.selection.to.x;
                    }
                    this.ctx.fillRect(from_x * this.cell_width, 
                        y * this.cell_height.height, 
                        (to_x - from_x + 1) * this.cell_width,
                        this.cell_height.height);
                }
            }
        }
        this.ctx.globalCompositeOperation = 'source-over';
    }
};

Screen.prototype.flush = function() {
    if (this.chars_buffer.length == 0) {
        return;
    }
    var buffer = this.chars_buffer;
    
    this.chars_buffer = '';
    
    var s = '';
    if (this.cursor.x > 0) {
        s = this.cells[this.cursor.y].substr(0, this.cursor.x);
    }
    s += buffer;
    if ((this.cursor.x  + buffer.length) < this.width) {
        s += this.cells[this.cursor.y].substr(this.cursor.x + buffer.length);
    }
    this.cells[this.cursor.y] = s;
    
    if (this.colors) {
        for (var x = this.cursor.x; x < this.cursor.x + buffer.length; x++) {
            this.cells_attributes[this.cursor.y][x] = this.cell_attributes;
        }
    }
    this.set_cursor(this.cursor.x + buffer.length, this.cursor.y);
    this.updated = false; 
};

Screen.prototype.clone_attributes = function() {
    var attributes;
    if (this.cell_attributes) {
        attributes = this.cell_attributes.clone();
    } else {
        attributes = this.default_cell_attributes.clone();
    }
    return attributes;
};

Screen.prototype.set_normal = function() {
    if (!this.colors) {
        return;
    }
    this.flush();
    this.cell_attributes = this.clone_attributes();
    this.cell_attributes.bold = false;
};

Screen.prototype.set_bold = function() {
    if (!this.colors) {
        return;
    }
    this.flush();
    this.cell_attributes = this.clone_attributes();
    this.cell_attributes.bold = true;
};

Screen.prototype.set_default_fg_color = function() {
    if (!this.colors) {
        return;
    }
    this.flush();
    this.cell_attributes = this.clone_attributes();
    this.cell_attributes.fg_color = this.default_cell_attributes.fg_color;
};

Screen.prototype.set_default_bg_color = function() {
    if (!this.colors) {
        return;
    }
    this.flush();
    this.cell_attributes = this.clone_attributes();
    this.cell_attributes.bg_color = this.default_cell_attributes.bg_color;
};

Screen.prototype.set_default_attributes = function() {
    this.flush();
    this.cell_attributes = undefined;
};

Screen.prototype.set_fg_color = function(color) {
    if (!this.colors) {
        return;
    }
    this.flush();
    this.cell_attributes = this.clone_attributes();
    this.cell_attributes.fg_color = color;
};

Screen.prototype.set_bg_color = function(color) {
    if (!this.colors) {
        return;
    }
    this.flush();
    this.cell_attributes = this.clone_attributes();
    this.cell_attributes.bg_color = color;
};

// add char to cursor position and increment cursor
Screen.prototype.add_char = function (c) {
    if (!this.equals_attributes(this.cell_attributes, this.ctx_cell_attributes)) {
        this.flush();
    }
    if ((this.cursor.x + this.chars_buffer.length) >= this.width) {
        // it calls flush
        this.set_cursor(0, this.cursor.y +1);
    }
    this.chars_buffer += c;
};

Screen.prototype.is_valid_cursor = function(x, y) {
    return x >=0 && x <= this.width && y >=0 && y < this.height; 
};

Screen.prototype.apply_cell_attributes = function(context,attributes) {
    this.flush();
    
    if (!this.equals_attributes(attributes, this.ctx_cell_attributes)) {
        if (attributes) {
            attributes.apply_diff(context, this.font_base, this.ctx_cell_attributes);
        } else {
            this.default_cell_attributes.apply_diff(context, this.font_base, this.ctx_cell_attributes);
        }
    } 
    this.ctx_cell_attributes = attributes;
};

// set the char in cursor position, without affecting the cursor position
Screen.prototype.set_char = function (c, attributes) {
    this.flush();
    // if a char is written outside the window I go to the next line, it happens within vim and bash itself
    if (this.cursor.x >= this.width) {
        this.set_cursor(0, this.cursor.y +1);
    }
    
    if (this.cursor.x <= this.width) {// && this.cursor.y < this.height) {
        var s = '';
        if (this.cursor.x > 0) {
            s = this.cells[this.cursor.y].substr(0, this.cursor.x);
        }
        s += c;
        if (this.cursor.x < this.width) {
            s += this.cells[this.cursor.y].substr(this.cursor.x +1);
        }
        this.cells[this.cursor.y] = s;

        if (this.cursor.x == this.width) {
            console('Screen: attempt to write "' + c + '" to (' + this.cursor.x + ', ' + this.cursor.y + ')', INFO);
        }
        if (this.colors) {
            this.cells_attributes[this.cursor.y][this.cursor.x] = attributes;
        }
    } else {
       console('Screen: attempt to write "' + c + '" to (' + this.cursor.x + ', ' + this.cursor.y + ')', INFO);
    } 
};

Screen.prototype.tab = function() {
    this.flush();
    this.set_cursor(this.cursor.x - this.cursor.x % 8 + 8, this.cursor.y);
};

// delete char in cursor position
Screen.prototype.delete_chars = function (count) {
    this.flush();
    var cursor_x = this.cursor.x;
    var cursor_y = this.cursor.y;
    
    var x;
    for (x = this.cursor.x; x < this.width-count; x++) {
        var c = this.cells[this.cursor.y].charAt(x + count);
        if (this.colors) {
            var attributes = this.cells_attributes[this.cursor.y][x + count];
            this.set_cursor(x, this.cursor.y);
            this.set_char(c, attributes);
        } else {
            this.set_cursor(x, this.cursor.y);
            this.set_char(c, undefined);
        }
    }
    
    for (; x < this.width; x++) {
        this.set_cursor(x, this.cursor.y);
        this.set_char(' ', undefined);
    }
    
    this.set_cursor(cursor_x, cursor_y);
};

Screen.prototype.redraw = function(context) {
    this.flush();
    this.udated = false;
};

Screen.prototype.draw = function(context) {
    this.flush();
    for (var y = 0; y < this.height; y++) {
        this.draw_line(context, y);
    }
};

Screen.prototype.insert_lines = function (count) {
    this.flush();
    var scroll_region = {first: this.scroll_region.first, last: this.scroll_region.last};
    
    this.scroll_region = {first: this.cursor.y, last: this.scroll_region.last};
    this.scroll_down(count);
    this.scroll_region = scroll_region;
};

Screen.prototype.delete_lines = function (count) {
    this.flush();
    var scroll_region = {first: this.scroll_region.first, last: this.scroll_region.last};
    
    this.scroll_region = {first: this.cursor.y, last: this.scroll_region.last};
    this.scroll_up(count, false);
    this.scroll_region = scroll_region;
};

Screen.prototype.redraw_line = function (context, line) {
    this.flush();
    context.clearRect(0, line * this.cell_height.height, this.canvas.width, (line + 1) * this.cell_height.height);
    this.draw_line(context,line);
};

Screen.prototype.equals_attributes = function (a1, a2) {
    if (a1) {
        if (a2) {
            return a1.equals(a2);
        } else {
            return a1.equals(this.default_cell_attributes);
        }
    } else if (a2) {
        return a2.equals(this.default_cell_attributes);
    } else {
        return true;
    }
};

Screen.prototype.draw_line = function (context, line) {
    this.flush();
    if (this.colors) {
        // I try to optimize, rendering text in chunks of equal attributes
        var current_x = 0;
        for (var x = 0; x < this.width; x++) {
            var attributes = this.cells_attributes[line][x];
            if (!this.equals_attributes(attributes, this.ctx_cell_attributes)) {
                context.fillText(this.cells[line].substr(current_x, x -current_x), current_x * this.cell_width, line * this.cell_height.height + 
                    this.cell_height.ascent);
                this.apply_cell_attributes(context, attributes);
                current_x = x;
            }
        }
        if (x - 1 > current_x) {
            context.fillText(this.cells[line].substr(current_x, x -current_x), current_x * this.cell_width, line * this.cell_height.height + 
                this.cell_height.ascent);
        }
    } else {
        context.fillText(this.cells[line], 0, line * this.cell_height.height + 
            this.cell_height.ascent);
    }
};

Screen.prototype.insert_chars = function (count) {
    this.flush();
    var s = '';
    if (this.cursor.x > 0) {
        s += this.cells[this.cursor.y].substr(0, this.cursor.x);
    }
    for (var i = 0; i < count; i++) {
        s += ' ';
    }
    if (this.cursor.x + count <= this.width) {
        s += this.cells[this.cursor.y].substr(this.cursor.x, this.width - (this.cursor.x + count) + 1);
    }
    
    this.cells[this.cursor.y] = s;
    
    // TODO colors
    if (this.colors) {
        log('insert_chars, colors not handled', ERROR);
    }
    
    // I don't move the cursor 
    //this.set_cursor(this.cursor.x + count, this.cursor.y);
};

/**
 * scroll_back true if it must put the scrolled region to the scroll buffer
 */
Screen.prototype.scroll_up = function (count, scroll_back) {
    this.flush();
    
    if (this.selection) {
        this.selection.from.y -= count;
        this.selection.to.y -= count;
    }
    
    var cursor_visible = this.cursor_visible;
    
    if (cursor_visible) {
        this.hide_cursor();
    }
    
    for (var y = this.scroll_region.first; y < this.scroll_region.last + 1 - count; y++) {
        if (scroll_back && (y - this.scroll_region.first) < count) {
            this.scroll_back_buffer.push(this.cells[y]);
            if (this.scroll_back_buffer.length > 1000) {
                this.scroll_back_buffer.shift();
            }
            this.scroll_back_line = this.scroll_back_buffer.length; 
        }
        this.cells[y] = this.cells[y + count]; 
        if (this.colors) {
            this.cells_attributes[y] = this.cells_attributes[y + count];
        }
    }
    
    for (var y = this.scroll_region.last + 1 - count; y < this.scroll_region.last + 1; y++) {
        this.cells[y] = this.empty_row;
        if (this.colors) {
            this.cells_attributes[y] = new Array(this.width +1);
        }
    }
/*
    var sr_height = this.scroll_region.last - this.scroll_region.first + 1;

    //this.tmp_canvas.width = this.tmp_canvas.width;
    this.tmp_ctx.clearRect(0, 0, this.tmp_canvas.width, this.back_canvas.height);

    this.tmp_ctx.drawImage(this.back_canvas, 0, 0);
    
    this.back_ctx.clearRect(0, (this.scroll_region.first) * this.cell_height.height, this.back_canvas.width, 
        this.cell_height.height * (this.scroll_region.last - this.scroll_region.first +1) -1); 
    
    this.back_ctx.drawImage(this.tmp_canvas, //img
        0, // sx 
        (this.scroll_region.first + count) * this.cell_height.height, // sy
        this.back_canvas.width, // swidth
        (this.scroll_region.last - this.scroll_region.first + 1 -count) *  this.cell_height.height, // sheight
        0, // x
        this.scroll_region.first * this.cell_height.height, // y
        this.back_canvas.width, // width
        (this.scroll_region.last - this.scroll_region.first + 1 -count) *  this.cell_height.height // height
    );
    if (cursor_visible) {
        this.show_cursor();
    }
*/
    this.cursor_visible = cursor_visible;
};

Screen.prototype.scroll_down = function (count) {
    this.flush();

    if (this.selection) {
        this.selection.from.y -= count;
        this.selection.to.y -= count;
    }

    var cursor_visible = this.cursor_visible;
    
    if (cursor_visible) {
        this.hide_cursor();
    }

    for (var y = this.scroll_region.last; y >= count; y--) {
        this.cells[y] = this.cells[y - count];
        if (this.colors) {
            this.cells_attributes[y] = this.cells_attributes[y - count];
        }
    }
    for (var y = this.scroll_region.first; y < this.scroll_region.first + count; y++) {
        this.cells[y] = this.empty_row;
        if (this.colors) {
            this.cells_attributes[y] = new Array(this.width +1);
        }
    }
/*
    var sr_height = this.scroll_region.last - this.scroll_region.first + 1;

    //this.tmp_canvas.width = this.tmp_canvas.width;
    this.tmp_ctx.clearRect(0, 0, this.tmp_canvas.width, this.back_canvas.height);
    
    this.tmp_ctx.drawImage(this.back_canvas, 0, 0);
    
    this.back_ctx.clearRect(0, (this.scroll_region.first) * this.cell_height.height, this.back_canvas.width, 
        this.cell_height.height * (this.scroll_region.last - this.scroll_region.first +1) -1); 
    
    this.back_ctx.drawImage(this.tmp_canvas, //img
        0, // sx 
        (this.scroll_region.first) * this.cell_height.height, // sy
        this.back_canvas.width, // swidth
        (this.scroll_region.last - this.scroll_region.first + 1 -count) *  this.cell_height.height, // sheight
        0, // x
        (this.scroll_region.first + count) * this.cell_height.height, // y
        this.back_canvas.width, // width
        (this.scroll_region.last - this.scroll_region.first + 1 -count) *  this.cell_height.height // height
    );
    if (cursor_visible) {
        this.show_cursor();
    }
*/
    this.cursor_visible = cursor_visible;
};

Screen.prototype.hide_cursor = function() {
    if (this.cursor_visible) {
        this.flush();
        if (this.cursor.x <= this.width) {
            this.updated = false;
        }
        this.cursor_visible = false;
    }
};

Screen.prototype.show_cursor = function() {
    if (!this.cursor_visible) {
	    this.flush();
        if (this.cursor.x <= this.width) {
            this.updated = false;
        }
        this.cursor_visible = true;
    }
};

Screen.prototype.set_cursor = function (x, y) {
    this.flush();
    if (x >= this.width && this.wrap_around) {
        x -= this.width;
        y++;
    }
    
    var cursor_visible = this.cursor_visible; 
    
    if (cursor_visible) {
        this.hide_cursor();
    }
    
    this.cursor.x = x;
    this.cursor.y = y;

    if (cursor_visible) {
        this.show_cursor();
    }    
    
    this.cursor_visible = cursor_visible;
    
    if (y > this.scroll_region.last) {
        // to handle infinite loop, since scroll_up can make a set_cursor
        this.cursor.y = this.scroll_region.last;
        this.scroll_up(y - this.scroll_region.last, true);
        this.set_cursor(x, this.scroll_region.last);
    }
//        console.warn('Screen: attempt to set cursor to (' + cell.x + ', ' + cell.y + ')');
    
};

Screen.prototype.cursor_right = function () {
    this.flush();
    this.set_cursor(this.cursor.x + 1, this.cursor.y);
};

Screen.prototype.carriage_return = function () {
    this.flush();
    this.set_cursor(0, this.cursor.y);
};

Screen.prototype.line_feed = function () {
    this.flush();
    // TODO I don't know if it's correct to got tho first column 
    this.set_cursor(0, this.cursor.y + 1);
};

Screen.prototype.clear = function (reset_cursor) {
    // TODO it's a trick to clear selection when running a program which clears the screen (vim, less, etc.) 
    this.set_selection(undefined);
    
    this.flush();
    this.cells = new Array();
    if (this.colors) {
	    this.cells_attributes = new Array();
	}
    for (var y=0; y <= this.height; y++) {
    /*
        var s = '';
        
        for (var x=0; x <= this.width; x++) {
            s += ' ';
        }
    */
        this.cells.push(this.empty_row);
        if (this.colors) {
	        this.cells_attributes.push(new Array(this.width +1));
	    }
    }
    
    if (reset_cursor) {
        this.set_cursor(0, 0);
    } else {
        this.set_cursor(this.cursor.x, this.cursor.y);
    }
};

Screen.prototype.erase_line_from_cursor = function () {
    this.flush();
    // TODO optimize with substr
    var s = '';
    for (var x = 0; x < this.width; x++) {
        if (x < this.cursor.x) {
            s += this.cells[this.cursor.y].charAt(x);
        } else {
            s += ' ';
            if (this.colors) {
                this.cells_attributes[this.cursor.y][x] = undefined;
            }
        }
    }
    this.cells[this.cursor.y] = s;

};

Screen.prototype.backspace = function () {
    this.flush();
    if (this.cursor.x > 0) {
        this.set_cursor(this.cursor.x - 1, this.cursor.y);
    // TODO must it go to prev line or the terminal does it?
    } else if (this.cursor.y > 0) {
        this.set_cursor(this.width - 1, this.cursor.y -1);
    }
};

Screen.prototype.up = function (count) {
    this.flush();
    // TODO optimize without a loop
    for (var i = 0; i < count; i++) {
    	if (this.cursor.y > this.scroll_region.first) {
    	   this.set_cursor(this.cursor.x, this.cursor.y - 1);
    	} else {
    	   this.scroll_down(1);
    	}
    }
};