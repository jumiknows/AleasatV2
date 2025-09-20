describe('Sponsors page', () => {
 
  beforeEach(() => {
    cy.visit("/sponsor");
    cy.wait(1000); // wait for components to mount 
  });


  // checks if click learn more it goes somewhere (for now can lead to "/")
  it('redirects to home when \'Learn More\' is clicked', () => {
    cy.url().should('eql', 'http://localhost:3000/sponsor');
    cy.get('[data-cy="sponsor-learnmore"]').click();
    cy.url().should('eql', 'http://localhost:3000/');
  })


  // checks when trying to go to another page from sponsor
  it('Can navigate to another page', () => {
    cy.url().should('eql', 'http://localhost:3000/sponsor');
    cy.get('[data-cy="navbar"]').within(() => {
      cy.get('[data-link="Team"]').click();
    })
    cy.url().should('eql', 'http://localhost:3000/team');
  })


  // checks for scrolling
  it('can scroll', () => {
    cy.visit('/sponsor'); 
    cy.get('[data-cy="navbar"]').scrollIntoView();
    cy.get('h4:first').scrollIntoView();
    cy.get('a[href*="https://www.esa.int/Education/Current_ESA_Member_States"]').scrollIntoView({timeout:200000});
    cy.get('footer').scrollIntoView();
   
  })
})