#!/bin/bash

ref_slug="$1"

cf_tunnel_config=$(curl --request GET \
  --url https://api.cloudflare.com/client/v4/accounts/$ACCOUNT_ID/cfd_tunnel/$TUNNEL_ID/configurations \
  --header "Authorization: Bearer $TOKEN" \
  --header 'Content-Type: application/json')

new_ingress=$(cat <<EOF
{"service":"http://$ref_slug.barbours-cut-review:3000","hostname":"$ref_slug-review.aleasat.space","originRequest":{}}
EOF
)

if [ $(echo "$cf_tunnel_config" | jq '.result.config.ingress | index('$new_ingress')') = "null" ]
then
  updated_cf_tunnel_config=$(echo "$cf_tunnel_config" | jq '.result.config.ingress |= ['$new_ingress'] + .' | jq '.result' )
  echo "Creating Ingress"
  curl --request PUT \
    --url https://api.cloudflare.com/client/v4/accounts/$ACCOUNT_ID/cfd_tunnel/$TUNNEL_ID/configurations \
    --header "Authorization: Bearer $TOKEN" \
    --header 'Content-Type: application/json' \
    --data "$updated_cf_tunnel_config" \
    > /dev/null
  curl --request POST \
    --url https://api.cloudflare.com/client/v4/zones/$ZONE_ID/dns_records \
    --header "Authorization: Bearer $TOKEN" \
    --header 'Content-Type: application/json' \
    --data '{
    "content": "'$TUNNEL_ID'.cfargotunnel.com",
    "name": "'$ref_slug'-review.aleasat.space",
    "proxied": true,
    "type": "CNAME",
    "ttl": 1
    }' \
    > /dev/null
else
  echo "Ingress already exists"
fi


