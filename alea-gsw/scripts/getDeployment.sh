#!/bin/bash

commit="$1"

cat <<EOF > deployment.yaml
apiVersion: apps/v1
kind: Deployment
metadata:
  name: "$commit"
  labels:
    app: "$commit"
  namespace: barbours-cut-review
spec:
  replicas: 1
  selector:
    matchLabels:
      app: "$commit"
  template:
    metadata:
      labels:
        app: "$commit"
    spec:
      containers:
        - name: "$commit"
          image: registry.gitlab.com/alea-2020/software/aleasat-software/aleasat-site-review:$commit
          ports:
            - containerPort: 3000
          envFrom:
            - secretRef:
                name: next-pw-protect-secret
          env:
            - name: AUTH_URL
              value: https://$commit-review.aleasat.space
            - name: AUTH_REDIRECT_PROXY_URL
              value: "https://www.aleasat.space/api/auth"
            - name: ENABLE_AUTH
              value: "true"
            - name: HOSTNAME
              value: "0.0.0.0"
          resources: {}
          imagePullPolicy: Always
      imagePullSecrets:
        - name: regcred
---
apiVersion: v1
kind: Service
metadata:
  labels:
    app: "$commit"
  name: "$commit"
  namespace: barbours-cut-review
spec:
  ports:
    - port: 3000
      name: http
      protocol: TCP
      targetPort: 3000
  selector:
    app: "$commit"
EOF