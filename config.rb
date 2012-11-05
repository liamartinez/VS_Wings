# Require any additional compass plugins here.

# Set this to the root of your project when deployed:
http_path = "/"
css_dir = "stylesheets"
sass_dir = "sass"
images_dir = "images"
javascripts_dir = "javascripts"

output_style = :nested
environment = :development

# To enable relative paths to assets via compass helper functions. Uncomment:
# relative_assets = true

# To disable debugging comments that display the original location of your selectors. Uncomment:
# line_comments = false
color_output = false


# If you prefer the indented syntax, you might want to regenerate this
# project again passing --syntax sass, or you can uncomment this:
# preferred_syntax = :sass
# and then run:
# sass-convert -R --from scss --to sass sass scss && rm -rf sass && mv scss sass


# This is common configuration
sass_dir = File.join 'views', 'stylesheets'
images_dir = File.join 'public', 'images'
http_path = "/"
http_images_path = "/images"
http_stylesheets_path = "/stylesheets"
output_style = :nested #:expanded #compressed #compact