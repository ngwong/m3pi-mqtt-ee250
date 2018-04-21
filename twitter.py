import json
import requests
import tweepy

def get_cur_loc():
	send_url = 'https://ipinfo.io'
	r = requests.get(send_url)
	return json.loads(r.text, "utf-8")

def get_api(cfg):
	auth = tweepy.OAuthHandler(cfg['consumer_key'], cfg['consumer_secret'])
	auth.set_access_token(cfg['access_token'], cfg['access_token_secret'])
	return tweepy.API(auth)

def main():
	# Fill in the values noted in previous step here
	cfg = {
		"consumer_key"			: "lG608JQsOPZrEUuLBGyW6OUv5",
		"consumer_secret"		: "vDNOXfjkXJ1NDVNMmO603uaMmmrhltj29NiUGBisSfJCScSO75",
		"access_token"			: "987595171914072064-nJcpTl7DP8cKe5alMgen6k6USmWVU84",
		"access_token_secret"	: "1OEVW9qw3HAHYtvbGqFF5sjJTLcw7EluMboCYYpGiJGAV"
	}

	api = get_api(cfg)
	cur_loc = get_cur_loc()
	#temp = get_temp()
	temp_msg = "N/A"

	org = "".join(cur_loc['org'].split()[1:-1])
	city = cur_loc['city']
	region = cur_loc['region']
	country = cur_loc['country']
	postal = cur_loc['postal']


	tweet = "It seems to be " + temp_msg + " in " + org + ", " + city + ", " + region + ", " + country + ", " + postal
	status = api.update_status(status=tweet)

if __name__ == "__main__":
	main()