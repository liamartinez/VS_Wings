require 'compass'
require 'sinatra'
require 'haml'

configure do
  Compass.add_project_configuration('config.rb')
  set :haml, { :format => :html5 }
end

get '/stylesheets/:name.css' do
  content_type 'text/css', :charset => 'utf-8'
  sass(:"stylesheets/#{params[:name]}", Compass.sass_engine_options)
end

get '/' do

  @body_class = "index"

  # array of all the image paths in the photos folder
  # with the "public/" part removed for links
  @images = Dir['public/images/*_large.jpg'].map do |f|
    full_path = f.gsub("public/", "")
    photo_name = full_path.gsub("photos/", "").gsub("_large", "")
    thumb_path = full_path.gsub("_large", "_thumb")
    file_size = File.stat(f).size
    { :full => full_path, :thumb => thumb_path,
      :size => file_size, :name => photo_name }
  end

  haml :index
end