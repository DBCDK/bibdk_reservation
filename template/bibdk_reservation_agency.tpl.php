    <div class="agency-container">

       <?php $branch = $variables['agency']; ?>
        <p><?php print $branch->branchName; ?></p>

        <?php if (isset($branch->postalAddress) && $branch->postalAddress) : ?>
          <p><?php print $branch->postalAddress; ?><br/>
            <?php print $branch->postalCode; ?> <?php print $branch->city; ?><br/>
            <?php $findVej = $branch->postalAddress.','.$branch->postalCode;?>
            <a href="http://findvej.dk/<?php print urlencode($findVej); ?>" target="_blank" />Se på kort (findvej.dk)</a></p>
        <?php endif; ?>

        <?php if (isset($branch->branchPhone) && $branch->branchPhone) : ?>
          <span>Telefon: <?php print $branch->branchPhone; ?></span>
        <?php endif; ?>

        <?php if (isset($branch->branchEmail) && $branch->branchPhone) : ?>
          <p>E-mail: <a href="mailto:<?php print $branch->branchEmail ?>"/><?php print $branch->branchEmail ?></a></p>
        <?php endif; ?>

        <?php if (isset($branch->branchWebsiteUrl) && $branch->branchWebsiteUrl) : ?>
          <p><a href="<?php print $branch->branchWebsiteUrl ?>" target="_blank" />Bibliotekets hjemmeside></a><br/>
          <?php endif; ?>
          <?php if (isset($branch->serviceDeclarationUrl) && $branch->serviceDeclarationUrl) : ?>
            <a href="<?php print $branch->serviceDeclarationUrl ?>" target="_blank" />Servicedeklaration></a></p>
        <?php endif; ?>
        <?php if (isset($branch->openingHours) && $branch->openingHours) : ?>
          <p>Åbningstider:<br/>
            <?php foreach ($branch->openingHours as $hours) : ?>
              <?php print $hours->{'$'}; ?>
              <br/>
            <?php endforeach; ?>
          </p>
        <?php endif; ?>