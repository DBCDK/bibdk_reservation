    <div class="agency-container">
      <h4>Du bestiller:</h4>
        <?php print $variables['manifestation']; ?>
      <h3>Personlige oplysninger</h3>
      <?php $user = $variables['user']; ?>
      <?php foreach ($user as $key => $value) : ?>
        <?php print $key . ': ' .$value . '<br />'; ?>
      <?php endforeach; ?>
      <br />
      <h3>Bibliotek</h3>
       <?php $branch = $variables['branch']; ?>
        <p><?php print $branch->branchName; ?><br/>
        <?php if (isset($branch->postalAddress) && $branch->postalAddress) : ?>
          <?php print $branch->postalAddress; ?><br/>
            <?php print $branch->postalCode; ?> <?php print $branch->city; ?><br/>
        <?php endif; ?>