import json
import pyowm
import requests
import tweepy

def convert(temp, unit):
    unit = unit.lower()
    if unit == "c":
        return 9.0 / 5.0 * temp + 32
    if unit == "f":
        return (temp - 32)  / 9.0 * 5.0

def get_heat_index(c_temp, humidity):
	# The input temperature is in celsius so convert to fahrenheit
	f_temp = convert(c_temp, 'C')

	# Calculate what the temperature actually feels like
	feel_temp = -42.379 + 									\
				(2.04901523*f_temp) + 						\
				(10.14333127*humidity) - 					\
				(0.22475541 * f_temp * humidity) - 			\
				(6.83783 * 10**-3 * f_temp**2) - 			\
				(5.481717 * 10**-2 * humidity**2 ) + 		\
				(1.22874 * 10**-3 * f_temp**2 * humidity) + 	\
				(8.5282 * 10**-4 * f_temp * humidity**2 ) - \
				(1.99 * 10**-6 ** f_temp**2 * humidity**2)

	# Return the converted value of the actual temperature in Celsius
	return convert(feel_temp, 'F')

# Returns the api object of twitter enabling tweet posting
def get_api(cfg):
	auth = tweepy.OAuthHandler(cfg['consumer_key'], cfg['consumer_secret'])
	auth.set_access_token(cfg['access_token'], cfg['access_token_secret'])
	return tweepy.API(auth)

# Extracts a json packet from the webiste and returns a json packet of strings
def get_cur_loc():
	send_url = 'https://ipinfo.io'
	r = requests.get(send_url)
	return json.loads(r.text, "utf-8")

# Extracts the actual and temperature of a place given the coordinates
def get_temp_and_humidity(lat, lon):
	owm = pyowm.OWM('13ff05b52e1127ce27d6c06b1b1cc411')
	w = owm.weather_at_coords(lat, lon).get_weather()
	return [w.get_temperature('celsius'), w.get_humidity()]
	# return [json.loads(w.get_temperature('celsius'), 'utf-8'), w.get_humidity()]


# Main function
def main():
	# Store the keys necessary to access the Twitter API
	cfg = {
		"consumer_key"			: "lG608JQsOPZrEUuLBGyW6OUv5",
		"consumer_secret"		: "vDNOXfjkXJ1NDVNMmO603uaMmmrhltj29NiUGBisSfJCScSO75",
		"access_token"			: "987595171914072064-nJcpTl7DP8cKe5alMgen6k6USmWVU84",
		"access_token_secret"	: "1OEVW9qw3HAHYtvbGqFF5sjJTLcw7EluMboCYYpGiJGAV"
	}

	# Get functions
	api = get_api(cfg)
	cur_loc = get_cur_loc()
	temp_and_humidity = get_temp_and_humidity(float(cur_loc['loc'].split(',')[0]), float(cur_loc['loc'].split(',')[1]))
	temp = temp_and_humidity[0]
	humidity = temp_and_humidity[1]
	# Compute the heat index based on the current temperature and humidity
	heat_index = 0
	temp_msg = ""

	# heat_index = get_heat_index(temp, humidity)
	# temp_msg = ""

	# if (heat_index < 0):
	# 	temp_msg = 'really cold'
	# elif (heat_index < 10):
	# 	temp_msg = 'cold'
	# elif (heat_index < 20):
	# 	temp_msg = 'warm'
	# elif (heat_index < 30):
	# 	temp_msg = 'hot'
	# else:
	# 	temp_msg = 'really hot'

	org = " ".join(cur_loc['org'].split()[1:])
	city = cur_loc['city']
	region = cur_loc['region']
	postal = cur_loc['postal']

	print temp['temp']

	# tweet = "It seems " + temp_msg + " in " + org + ", " + city + ", " + region + " " + postal + '\n\n' \
	# 		"Outside: T = " + temp + " C, H = " + humidity + "%, Feels like " + heat_index + " C"
	# status = api.update_status(status=tweet)

if __name__ == "__main__":
	main()