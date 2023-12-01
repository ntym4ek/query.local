<?php

/**
 * @file
 * Default simple view template to display a list of rows.
 *
 * @ingroup views_templates
 */
$nom = 0;
$first = $last = false;
?>
<?php if (!empty($title)): ?>
  <h3><?php print $title; ?></h3>
<?php endif; ?>
<?php foreach ($rows as $index => $row): ?>
  <?php
    if ($nom != $view->result[$index]->field_field_nomenklatura[0]["raw"]["target_id"]) {
      $nom = $view->result[$index]->field_field_nomenklatura[0]["raw"]["target_id"];
      $first = true; $last = false;
      $id = $view->result[$index]->id;
    } else {
      $first = false;
    }
    if (empty($view->result[$index+1]) || $nom != $view->result[$index+1]->field_field_nomenklatura[0]["raw"]["target_id"]) {
      $last = true;
    }
  ?>

  <div<?php if ($classes_array[$index]): ?> class="<?php print $classes_array[$index]; ?>"<?php endif; ?>>
    <?php print $row; ?>
  </div>

  <?php if ($first): ?>
    <div class="collapse" id="collapse-<? print $id; ?>">
  <?php endif; ?>

  <?php if ($last): ?>
    </div>
  <?php endif; ?>

<?php endforeach; ?>
