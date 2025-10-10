#!/bin/bash

ref_slug="$1"

dns_record=$(curl --request GET \
  --url https://api.cloudflare.com/client/v4/zones/$ZONE_ID/dns_records?name=$ref_slug-review.aleasat.space \
  --header "Authorization: Bearer $TOKEN" \
  --header 'Content-Type: application/json')

dns_record_id=$(echo "$dns_record" | jq -r '.result[0].id')

curl --request DELETE \
  --url https://api.cloudflare.com/client/v4/zones/$ZONE_ID/dns_records/$dns_record_id \
  --header "Authorization: Bearer $TOKEN" \
  --header 'Content-Type: application/json' \
  > /dev/null

cf_tunnel_config=$(curl --request GET \
  --url https://api.cloudflare.com/client/v4/accounts/$ACCOUNT_ID/cfd_tunnel/$TUNNEL_ID/configurations \
  --header "Authorization: Bearer $TOKEN" \
  --header 'Content-Type: application/json')

new_ingress=$(cat <<EOF
{"service":"http://$ref_slug.barbours-cut-review:3000","hostname":"$ref_slug-review.aleasat.space","originRequest":{}}
EOF
)

updated_cf_tunnel_config=$(echo "$cf_tunnel_config" | jq '.result | del( .config.ingress[] | select(.hostname == "'$ref_slug'-review.aleasat.space") )')

curl --request PUT \
  --url https://api.cloudflare.com/client/v4/accounts/$ACCOUNT_ID/cfd_tunnel/$TUNNEL_ID/configurations \
  --header "Authorization: Bearer $TOKEN" \
  --header 'Content-Type: application/json' \
  --data "$updated_cf_tunnel_config" \
  > /dev/null
