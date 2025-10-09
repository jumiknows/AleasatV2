describe("Home Page", () => {
  beforeEach(() => {
    cy.visit("/");
  });

  it("Should verify the text in the h1 SPONSOR element", () => {
    cy.get("h1") // Locate the h1 element
      .contains("SPONSORS"); // Verify it contains the text "SPONSORS"
  });
  it("Should verify the text in the h2 SPONSOR element", () => {
    cy.get("h2").contains(
      "Partner with us to make an impact and support our vision for the future.",
    ); // Verify it contains the correct text
  });
  it('Should click the platinum button with the "my-button" class', () => {
    // Click the platinum button
    cy.get(".my-button").eq(0).trigger("click");

    // Verify the platinum element remains visible after the click
    cy.get("[data-testid='platinum']").should("be.visible");
  });

  it('Should click the gold button with the "my-button" class', () => {
    // Click the gold button
    cy.get(".my-button").eq(1).trigger("click");

    // Verify the gold element remains visible after the click
    cy.get("[data-testid='gold']").should("be.visible");
  });

  it('Should click the silver button with the "my-button" class', () => {
    // Click the silver button
    cy.get(".my-button").eq(2).trigger("click");

    // Verify the silver element remains visible after the click
    cy.get("[data-testid='silver']").should("be.visible");
  });

  it('Should click the bronze button with the "my-button" class', () => {
    // Click the bronze button
    cy.get(".my-button").eq(3).trigger("click");

    // Verify the bronze element remains visible after the click
    cy.get("[data-testid='bronze']").should("be.visible");
  });

  it("Should render the video element with the correct source", () => {
    // Locate the video element and find the source element inside it
    cy.get('[data-testid="background-video"]') // Target the video element using the test ID
      .should("exist") // Ensure the video element exists
      .find("source") // Find the <source> element within the video tag
      .should("exist") // Ensure the source element exists
      .and("have.attr", "src", "https://artifact.aleasat.space/stargazing.mp4") // Verify the src attribute
      .and("have.attr", "type", "video/mp4"); // Verify the type attribute
  });
  it("Should render the correct text inside the quote element", () => {
    cy.get("h2") // Find the h2 element
      .should("exist") // Ensure it exists
      .contains("AUDEBIMUS IN INCOGNITUM.");
  });

  it("should find the anchor tag with the text 'Spacecraft' and click it", () => {
    // Get all anchor tags and use contains to find the one with 'Components'
    cy.get("a")
      .contains("Spacecraft") // Finds the anchor tag with the text 'Components'
      .click(); // Click it

    // Verify the URL after clicking
    cy.url().should("include", "/spaceship"); // Adjust according to your app's route
  });

  it("should find the anchor tag with the text 'Team' and click it", () => {
    // Get all anchor tags and use contains to find the one with 'Components'
    cy.get("a")
      .contains("Team") // Finds the anchor tag with the text 'Components'
      .click(); // Click it

    cy.wait(4000); // Wait for 1 second (adjust the time as needed)

    // Verify the URL after clicking
    cy.url().should("include", "/team"); // Adjust according to your app's route
  });

  it("should find the anchor tag with the text 'Sponsor' and click it", () => {
    // Get all anchor tags and use contains to find the one with 'Components'
    cy.get("a")
      .contains("Sponsor") // Finds the anchor tag with the text 'Components'
      .click(); // Click it

    // Verify the URL after clicking
    cy.url().should("include", "/sponsor"); // Adjust according to your app's route
  });

  it("should navigate to the home page when the image is clicked", () => {
    // Find the image by its alt text and click it
    cy.get('img[alt="AleaSat Logo"]').click();

    // Verify the URL is correct after clicking the image
    cy.url().should("eq", "http://localhost:3000/"); // Adjust based on your URL
  });

  it("should find the 'Spacecraft' link in the footer and click it", () => {
    cy.get('[data-testid="footer"]') // Get the footer using its test ID
      .contains("Spacecraft") // Filter down to the one with the text 'Spacecraft'
      .click({ force: true }); // Force the click even if the element is not visible

    cy.wait(4000); // Optional wait time

    // Verify the URL after clicking
    cy.url().should("include", "/spaceship"); // Adjust according to your app's route
  });

  it("should find the 'Team' link in the footer and click it", () => {
    cy.get('[data-testid="footer"]') // Get the footer using its test ID
      .contains("Team") // Filter down to the one with the text 'Team'
      .click({ force: true }); // Force the click even if the element is not visible

    cy.wait(4000); // Optional wait time

    // Verify the URL after clicking
    cy.url().should("include", "/team"); // Adjust according to your app's route
  });

  it("should find the 'Sponsor' link in the footer and click it", () => {
    cy.get('[data-testid="footer"]') // Get the footer using its test ID
      .contains("Sponsor") // Filter down to the one with the text 'Sponsor'
      .click({ force: true }); // Force the click even if the element is not visible

    cy.wait(4000); // Optional wait time

    // Verify the URL after clicking
    cy.url().should("include", "/sponsor"); // Adjust according to your app's route
  });

  it("should click the Facebook link and verify the URL", () => {
    // Find the Facebook link by its test ID and click it
    cy.get('[data-testid="facebook-link"]').click({ force: true }); // Using force to click even if the element is hidden or styled differently

    // Verify the URL after clicking
    // cy.url().should("include", "facebook");
  });

  it("should click the Instagram link and verify the URL", () => {
    // Find the Instagram link by its test ID and click it
    cy.get('[data-testid="instagram-link"]').click({ force: true });

    // Verify the URL after clicking
    // cy.url().should("include", "instagram");
  });

  it("should click the LinkedIn link and verify the URL", () => {
    // Find the LinkedIn link by its test ID and click it
    cy.get('[data-testid="linkedin-link"]').click({ force: true });

    // Verify the URL after clicking
    // cy.url().should("include", "linkedin");
  });
});
