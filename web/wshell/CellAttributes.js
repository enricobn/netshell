function CellAttributes() {
};

CellAttributes.prototype.apply = function(ctx, font_base) {
    var font = font_base;
    
    if (this.bold) {
        font = 'bold ' + font;
    }
    
    if (this.italic) {
        font = 'italic ' + font;
    }
    
    ctx.font = font;
    ctx.fillStyle = this.fg_color;
};

CellAttributes.prototype.equals = function(attributes) {
    return this.bold == attributes.bold && this.italic == attributes.italic &&
        this.fg_color == attributes.fg_color && this.bg_color == attributes.bg_color;
};

// applies only changed attributes 
CellAttributes.prototype.apply_diff = function(ctx, font_base, actual_attributes) {
    if (actual_attributes) {
        if (this.bold != actual_attributes.bold || this.italic != actual_attributes.italic) { 
            var font = font_base;
            
            if (this.bold) {
                font = 'bold ' + font;
            }
            
            if (this.italic) {
                font = 'italic ' + font;
            }
            
            ctx.font = font;
        }
        
        if (this.fg_color != actual_attributes.fg_color) {
            ctx.fillStyle = this.fg_color;
        }
    } else {
        this.apply(ctx, font_base);
    }
};

CellAttributes.prototype.clone = function() {
    var attributes = new CellAttributes();
    attributes.bold = this.bold;
    attributes.italic = this.italic;
    attributes.fg_color = this.fg_color;
    attributes.bg_color = this.bg_color;
    
    return attributes; 
};

// true / false
CellAttributes.prototype.bold;
// true / false
CellAttributes.prototype.italic;
// string
CellAttributes.prototype.fg_color;
// string
CellAttributes.prototype.bg_color;