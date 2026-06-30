set pagination off
set confirm off
set print pretty on
set print array on

set breakpoint pending on

break ff_open_png
break ff_png_isvalid
break ff_png_dispatch
break ff_png_header_handler
break ff_png_data_handler
break ff_png_end_handler

define ff
    run
end

define st
    info locals
    info args
end

define ihdr
    print png_ctx->width
    print png_ctx->height
    print png_ctx->bit_depth
    print png_ctx->color_type
end

define ctx
    print *ctx
end

define png
    print *png_ctx
end
