require 'rubygems'
require 'sinatra'

# require 'capture_stdout'
# require 'escape_utils'
require 'slim'

require 'haml'
require 'compass'
require 'aws/s3'
# require 'bootstrap-sass'

# require 'eventmachine'
# require 'happening'

configure do
  Compass.add_project_configuration('config.rb')
  set :haml, { :format => :html5 }
end

get '/stylesheets/:name.css' do
  content_type 'text/css', :charset => 'utf-8'
  sass(:"stylesheets/#{params[:name]}", Compass.sass_engine_options)
end

include AWS::S3

  # Defines a GET action at "/hello"
  # get '/hello' do
  #   big_job = lambda { sleep 10 }
  #   result = lambda { 'Hello!' }
  #   EM.defer big_job, result
  # end

  # Defines a POST action at "/bienvendidos"
  # post '/bienvendidos' do
  #   'Bienvendidos!'
  # end


def s3_connect
  Base.establish_connection!(
    :access_key_id     => 'AKIAIZCSX656MZOD362A',
    :secret_access_key => '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'
  )
end

get '/' do
  s3_connect
  @bucket = Bucket.find('wings_victoria_')
  haml :index
end