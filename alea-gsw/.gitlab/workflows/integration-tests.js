export default {
  "launchpad": {
    variables: {
      "POSTGRES_USER": "launchpad",
      "POSTGRES_PASSWORD": "password123",
      "POSTGRES_DB": "launchpad",
      "POSTGRES_URL": "postgresql://launchpad:password123@postgres:5432/launchpad"
    },
    services: [
      // services:
      // - name: my-postgres:11.7
      //   alias: db,postgres,pg
      //   entrypoint: ["/usr/local/bin/db-postgres"]
      //   command: ["start"]
      // See: https://docs.gitlab.com/ee/ci/services/#define-services-in-the-gitlab-ciyml-file
      {
        // Image name
        "name": "postgres",
        // url/hostname for accessing the service
        "alias": "postgres"
      }
    ]
  }
}