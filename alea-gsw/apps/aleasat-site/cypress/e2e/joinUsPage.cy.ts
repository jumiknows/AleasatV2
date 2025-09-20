describe("Test Join Us Page", () => {
  beforeEach(() => {
    cy.visit("/join-us"); 
  });

  it("should contain NavBar and click to different page", () => {
    cy.get('[data-cy="navbar-Sponsor"]').click();
    cy.screenshot();
  });  

  it("should scroll on the page", () => {
    cy.window().then((window) => {
      const hasScrollableContent = window.document.body.scrollHeight > window.innerHeight;
      expect(hasScrollableContent).to.be.true; 
    });
    cy.screenshot();
  });

  it('should display the correct subteam information when the "ADMIN" button is clicked', () => {
    // Assert that the "ADMIN TEST" position card is displayed correctly
    cy.get('[data-cy="join-us-position-card-ADMIN TEST-7"]').should('be.visible'); // Adjust the index if needed
    cy.get('[data-cy="join-us-position-card-ADMIN TEST-7"]').click();

    // Assert that the description for "ADMIN TEST" is visible
    cy.get('[data-cy="join-us-position-description-ADMIN TEST"]').contains('The Graphic Designer produces social media posts, posters, merchandise designs, and other promotional content for the UBC Orbit Satellite Design Team.').should('be.visible');
    
    // Assert that the skills "Figma", "Canvas", "Photoshop" are visible
    cy.get('[data-cy="join-us-skill-Figma-0-ADMIN TEST"]').should('be.visible');
    cy.get('[data-cy="join-us-skill-Canvas-1-ADMIN TEST"]').should('be.visible');
    cy.get('[data-cy="join-us-skill-Photoshop-2-ADMIN TEST"]').should('be.visible');
  
    cy.screenshot();
  });
  
  it("should redirect to the correct link when clicking 'Apply Now'", () => {
    // Assert that the "Apply Now" link has the correct URL
    cy.get('[data-cy="join-us-apply-link"]')
      .should("have.attr", "href", "https://aleasat.space"); 
  
    cy.screenshot();
  });
  
  });
  