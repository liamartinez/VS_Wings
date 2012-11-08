require 'rubygems'
require 'sinatra'
require 'haml'
require 'aws/s3'


# Set Sinatra variables
set :app_file, __FILE__
set :root, File.dirname(__FILE__)
set :views, 'views'
set :public_folder, 'public'
set :haml, {:format => :html5} # default Haml format is :xhtml

# Application routes
get '/' do

  @body_class = "index"

  @images = Dir['public/images/*_large.JPG', 'public/images/*_large.jpg'].map do |f|
    full_path = f.gsub("public/", "")
    photo_name = full_path.gsub("images/", "").gsub("_large", "")
    thumb_path = full_path.gsub("_large", "_thumb")
    file_size = File.stat(f).size
    { :full => full_path, :thumb => thumb_path,
      :size => file_size, :name => photo_name }
  end


  haml :index, :layout => :'layouts/application'
end

get '/about' do
  haml :about, :layout => :'layouts/page'
end